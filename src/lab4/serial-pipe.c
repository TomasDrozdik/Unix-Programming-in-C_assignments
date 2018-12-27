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
	int i, pid, stat_loc, cmds_count = argc - 1;
	int prev_pipe[2], next_pipe[2];
	char **cmds = argv + 1;

	if (argc < 2) {
		errx(1, "usage: ./a.out <cmd> ...");
	}

	if (pipe(next_pipe) == -1) {
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
				dup2(prev_pipe[0], 0);
				CLOSE_PIPE(prev_pipe);
			}

			/* redirect output */
			if (i != cmds_count - 1) { /* exclude last cmd */
				dup2(next_pipe[1], 1);
				CLOSE_PIPE(next_pipe);
			}

			fprintf(stderr, "Child %d execing %s\n", getpid(), cmds[i]);
			execlp(cmds[i], cmds[i], NULL);
			err(1, "execlp");
		default:
			fprintf(stderr, "Spawning process %d\n", pid);

			CLOSE_PIPE(prev_pipe);
			prev_pipe[0] = next_pipe[0];
			prev_pipe[1] = next_pipe[1];
			if (pipe(next_pipe) == -1) {
				err(1, "pipe");
			}
		}
	}

	CLOSE_PIPE(prev_pipe);
	CLOSE_PIPE(next_pipe);

	fprintf(stderr, "Main waiting for pid %d\n", pid);
	waitpid(pid, &stat_loc, 0);
	return (0);
}