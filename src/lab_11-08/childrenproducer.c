/* childrenproducer.c */

/*
 *  - fork a child, fork again in the child, and again until specified count
 *    of children is met.
 *  - Every parent waits for its child.
 *  - Exec pstree(1) with -psa options in the last child on itself
 *    (i.e. `pstree -psa <childs-pid>`).
 *  - Exit then.
 *  ```
 *  usage: ./a.out <num_children>
 *  ```
 *  - on Solaris use `ptree <pid>`, on macOS install `pstree` from Homebrew
 *  - the goal is to create a cascade of processes, not flat tree ("broom")
 */

#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int
main(int argc, char *argv[])
{
	int i, pid, awaited_pid;

	if (argc < 2) {
		fprintf(stderr, "usage: ./a.out <num_children>");
		return (-1);
	}

	i = atoi(argv[1]);

	while (i-- != 0) {
		switch (pid = fork()) {
		case -1:
			err(-1, "fork()");
		case 0:
			printf("[%d]Child spawned whith idx = %d\n",
				getpid(), i);
			sleep(1);
			continue;
		default:
			printf("[%d]Parent spawned process %d\n",
				getpid(), pid);
			awaited_pid = waitpid(pid, NULL, 0);
			printf("[%d]Parent waited for process %d\n",
				getpid(), awaited_pid);
			goto parent_escape;
		}
	}
parent_escape:
	return (0);
}