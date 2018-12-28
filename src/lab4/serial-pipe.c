/* serial-pipe.c */

/*
 * Implement a serial pipe so that main process forks argc - 1 times
 * usage:
 * ./a.out date cat cat wc
 */

#define CLOSE_PIPE(pd) close(pd[0]); close(pd[1]);

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
	int i, pid, previous_read_end, cmds_count = argc - 1;
	int pd[2];
	char **cmds = argv + 1;

	if (argc < 2) {
		errx(1, "usage: ./a.out <cmd> ...");
	}

	if (pipe(pd) == -1) {
		err(1, "pipe");
	}

	/* fork processes */
	for (i = 0; i < cmds_count; ++i) {
		switch (pid = fork()) {
		case -1:
			err(1, "fork");
		case 0:
			/* redirect input */
			if (i != 0) { /* exclude first cmd */
				dup2(previous_read_end, 0);
				close(previous_read_end);
			}

			/* redirect output */
			if (i != cmds_count - 1) { /* exclude last cmd */
				dup2(pd[1], 1);
				CLOSE_PIPE(pd);
			}

			execlp(cmds[i], cmds[i], NULL);
			err(1, "execlp");
		default:
			close(previous_read_end);
			close(pd[1]);

			previous_read_end = pd[0];

			if (pipe(pd) == -1) {
				err(1, "pipe");
			}
		}
	}

	CLOSE_PIPE(pd);
	close(previous_read_end);

	waitpid(pid, NULL, 0);
	return (0);
}