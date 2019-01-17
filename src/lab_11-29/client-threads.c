/* client.c */

/*
 * TODO: similarly to client-select.c cant properly exit. after connection is
 * over you have to type something so the block on read would end and next loop
 * of while in rdfrom_wrto() in thread will notice change in variable
 * thread_finished.
 */

#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libgen.h>
#include <unistd.h>

#define	BUF_LEN 1024

/*
 * Global variabe to monitor whether one thread finished. No synchronization
 * primitive needed since another looop of thread doestn't matter.
 */
int thread_finished = 0;

struct fds
{
	int reader_fd, writer_fd;
};

void
usage(const char *prog_name)
{
	printf("Usage: %s <address> <port>",
		prog_name);
}

/*
 * Check if arguments are in correct format and fill addrstr, portstr.
 */
void
parse_args(int argc, char **argv, char **addrstr, char **portstr)
{
	if (argc != 3) {
		usage(basename(argv[0]));
		exit(1);
	}

	*addrstr = argv[1];
	*portstr = argv[2];
}

int
create_connection(char *addrstr, char *portstr)
{
	int fd, ret;
	struct addrinfo *res, *resorig, hints;

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((ret = getaddrinfo(addrstr, portstr, &hints, &res)) != 0)
		errx(1, "getaddrinfo: %s", gai_strerror(ret));

	for (resorig = res; res != NULL; res = res->ai_next) {
		if ((fd = socket(res->ai_family, res->ai_socktype,
				res->ai_protocol)) == -1)
			err(1, "socket");

		if (connect(fd, (struct sockaddr *)res->ai_addr, res->ai_addrlen) == 0)
			break;
	}
	freeaddrinfo(resorig);

	return (fd);
}

void *
rdfrom_wrto(void *struct_fdsp)
{
	int sz;
	char buf[BUF_LEN];
	struct fds *fds = (struct fds *)struct_fdsp;

	while (!thread_finished && (sz = read(fds->reader_fd, buf, BUF_LEN)) > 0) {
		/* Check if other thread finished. */
		write(fds->writer_fd, buf, sz);
	}

	/* To signal to other thread that this one is finished. */
	thread_finished = 1;

	return NULL;
}

void
run(int sock)
{
	int error;
	pthread_t th1, th2;
	struct fds fds1, fds2;

	fds1.reader_fd = 0;
	fds1.writer_fd = sock;
	if ((error = pthread_create(&th2, NULL, rdfrom_wrto, &fds1)) != 0)
			errx(1, "pthread_create: %s", strerror(error));

	fds2.reader_fd = sock;
	fds2.writer_fd = 1;
	if ((error = pthread_create(&th1, NULL, rdfrom_wrto, &fds2)) != 0)
			errx(1, "pthread_create: %s", strerror(error));

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
}

int
main(int argc, char **argv)
{
	int sock;
	char *addrstr, *portstr;

	parse_args(argc, argv, &addrstr, &portstr);

	sock = create_connection(addrstr, portstr);

	run(sock);

	close(sock);
}
