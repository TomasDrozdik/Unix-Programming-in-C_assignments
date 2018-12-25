/* recursive-pipe.c */

/*
 * Implement a recursive pipepile.
 * Usage: ./a.out date cat cat cat sort
 * OUT: Thu Nov 15...
 * Main should fork just once.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void
do_child(char **argv, int idx, int prev_pd[2])
{
	int next_pd[2];

	if (prev_pd != NULL) { /* last cmd => don't redirect output */
		dup2(prev_pd[1], 1); /* redirect output */
		close(prev_pd[0]);
		close(prev_pd[1]);
	}

	if (idx == 1) { /* first cmd in pipeline => execute */
		execlp(argv[idx], argv[idx], NULL);
		err(1, "execlp");
	}

	if (pipe(next_pd) == -1) { /* create a new pipe */
		err(1, "pipe");
	}

	switch (fork()) {
	case -1:
		err(1, "fork");
	case 0:	/* child */
		do_child(argv, --idx, next_pd);
	default: /* parent */
		dup2(next_pd[0], 0);
		close(next_pd[0]);
		close(next_pd[1]);
		execlp(argv[idx], argv[idx], NULL);
		err(1, "execlp");
	}
}

int
main(int argc, char **argv)
{
	do_child(argv, argc - 1, NULL);

	return (0);
}
