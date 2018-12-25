/* daemon.c */

/*
 * -reimplement daemon(3)
 * -basic idea: fork a new child, exit the parent
 * -make it robust w.r.t. setsid(2) failure
 * -make sure to behave correctly w.r.t. file descriptors (verify with lsof(1))
 * -write tests
 */

#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

void
daemon(int nochdir, int noclose)
{
	int pid, devnullfd;

	switch (pid = fork()) {
	case -1:
		err(-1, "fork");
	case 0:
		break;
	default:
		fprintf(stderr, "[%d]Parent spawned daemon %d\n",
			getpid(), pid);
		exit(0);
	}

	int session_id = setsid();

	fprintf(stderr, "[%d]Deamon with sessionid = %d\n",
		getpid(), session_id);

	if (nochdir == 0) {
		if (chdir("/") == -1) {
			err(-1, "chdir");
		}
	}

	if (noclose == 0) {
		fprintf(stderr,
			"Redirecting stdin stdout stderr -> /dev/null\n");

		if ((devnullfd = open("/dev/null", O_RDWR)) == -1) {
			err(-1, "open");
		}

		dup2(devnullfd, 0);
		dup2(devnullfd, 1);
		dup2(devnullfd, 2);
	}
}


int
main(void)
{
	daemon(0, 0);

	while (1) {
		sleep(1);
	}

	return (0);
}