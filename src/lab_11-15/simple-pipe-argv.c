/* simple-pipe-argv.c */

/*
 * Same as simple-pipe but use argv to specify command of the child.
 */

#define _XOPEN_SOURCE 700

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define ARRLEN(arr) sizeof (arr) / sizeof (arr[0])

int
main(int argc, char *argv[])
{
	pid_t pid;
	int pd[2];

	if (pipe(pd) == -1) {
		err(1, "pipe");
	}

	switch (pid = fork()) {
	case -1:
		err(1, "fork");
	case 0:
		close(1);
		dup(pd[1]);
		close(pd[0]);
		close(pd[1]);
		execvp(argv[1], argv + 1);
		err(1, "execv");
	default:
		close(0);
		dup(pd[0]);
		close(pd[0]);
		close(pd[1]);

#if 0
		execl("/usr/bin/less", "echo", NULL);
		err(1, "execl");
#elif 1
		char buf[10];
		ssize_t len;
		while ((len = read(0, buf, ARRLEN(buf))) != -1 &&
			len != 0) {
			if (write(1, buf, len) != len) {
				err(1, "write");
			}
		}
#endif
	}

	return (0);
}