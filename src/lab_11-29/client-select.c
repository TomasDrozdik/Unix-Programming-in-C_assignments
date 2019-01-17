/* client.c */

#include <err.h>
#include <fcntl.h>
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

void
run(int sock)
{
	int sz;
	char buf[BUF_LEN];
	fd_set rdfds;;

	while (1) {
		FD_ZERO(&rdfds);
		FD_SET(0, &rdfds);
		FD_SET(sock, &rdfds);

		if (select(sock + 1, &rdfds, NULL, NULL, NULL) == -1)
			err(1, "select");

		if (FD_ISSET(sock, &rdfds)) {
			if ((sz = read(sock, buf, BUF_LEN)) > 0)
				write(1, buf, sz);

			/* If the socket is closed. */
			if (sz <= 0)
				break;
		} else if (FD_ISSET(0, &rdfds)) {
			if ((sz = read(0, buf, BUF_LEN)) > 0)
				write(sock, buf, sz);

			/* If the client send EOF from stdin. */
			if (sz == -1)
				break;
		}
	}
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
