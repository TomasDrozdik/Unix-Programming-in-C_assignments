/* simple-fork.c */

/*
  - Fork a process, sleep in the child for specified timeout using sleep(3).
  - In the parent, wait for the child, exit.
  - see what happens if parent does not wait on the child to finish
*/

#include <err.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(void)
{
	int pid, stat, stopped_pid;

	switch (pid = fork()) {
	case -1:
		err(-1, "fork");
	case 0:
		printf("%d: sleeping in child\n", getpid());
#if 0
       	while (1) {
			sleep(3);
		}
#else
        execlp("sleep", "sleep", "100", NULL);
#endif
	default:
		printf("%d: Parent waiting for child %d\n", getpid(), pid);
		stopped_pid = waitpid(pid, &stat, 0);
		printf("%d: wait returnned status %d\n", stopped_pid, stat);
		break;
	}

	return 0;
}