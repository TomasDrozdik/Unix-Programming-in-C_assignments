/* read-blocking.c */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int
main(void)
{
	int reading_pd[2];

	pipe(reading_pd);

	switch (fork()) {
	case 0:
		dup2(reading_pd[0], 0);

		close(reading_pd[0]);
		close(reading_pd[1]);

		char c;
		ssize_t readlen;

		while ((readlen = read(0, &c, 1)) != -1 &&
		  readlen != 0) {
			write(1, &c, 1);
		}
		return (0);
	default:
		close(reading_pd[0]);
	}

	char *msg = "HelloWorld";
	write(reading_pd[1], msg, strlen(msg));
	close(reading_pd[1]);

	wait(NULL);

	return (0);
}