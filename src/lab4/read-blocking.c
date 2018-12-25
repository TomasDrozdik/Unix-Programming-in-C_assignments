/* read-blocking.c */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int
main(void)
{
	char buf[4096];
	ssize_t readlen;

	while ((readlen = read(0, buf, 4096)) != -1) {
		write(1, buf, readlen);
	}

	return (0);
}