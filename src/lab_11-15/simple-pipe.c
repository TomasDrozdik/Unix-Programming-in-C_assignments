/* simple-pipe.c */

/*
 * Fork a process, exec e.g. date, read the child's output through
 * the pipe but use fd 0!
 * Print output to stdout;
 */

#define _XOPEN_SOURCE 700

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define ARRLEN(arr) sizeof (arr) / sizeof (arr[0])

int
main(void)
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
		execl("/bin/date", "date", NULL);
		err(1, "execl");
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