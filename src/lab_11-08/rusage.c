/* rusage.c */

/*
 * -parent creates a child that will allocate specified number of memory on the heap,
 *  fill it with given byte and exit.
 * -Parent will then report high-water memory usage using wait4().
 * -see getrusage(2) man page for details about the rusage structure members
 * -try the same via GNU time (e.g. `/usr/bin/time -v -- /some/program`) and see
 * -if the reported values match
 * -see https://github.com/gsauthof/cgmemtime for more sophisticated solution
 */

#define _DEFAULT_SOURCE	;

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(void)
{
	int pid;
	char *arr[1024];
	struct rusage ru;

	switch (pid = fork()) {
	case -1:
		err(-1, "fork");
		break;
	case 0:
		*arr = (char *)malloc(sizeof (char[1024]));
		printf("Child allocated memory on heap %zu\n", sizeof (*arr));
		break;
	default:
		printf("Parent waiting for child %d\n", pid);
		wait4(pid, NULL, 0, &ru);

		printf("User CPU time used %ld sec | %ld usec\n",
			ru.ru_utime.tv_sec, ru.ru_utime.tv_usec);

		printf("System CPU time used %ld sec | %ld usec\n",
			ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);

		printf("Maximum resident set size %ld\n",
			ru.ru_maxrss);

		free (*arr);
		break;
	}

	return (0);
}