/* exec-memmory.c */

/*
 * A check whether alocated memmory passed to exec appears at valgrind.
 */

/*
 * Update:
 * No in fact valgrind detects freed memmory properly.
 */

#define ARGC 3

#include <err.h>
#include <stdlib.h>
#include <unistd.h>


int
main(void)
{
	char **argv = malloc(sizeof (char *) * 3);

	argv[0] = "/bin/echo";
	argv[1] = "HelloWorld";
	argv[2] = NULL;

	switch (fork()) {
	case -1:
		err(-1, "fork");
	case 0:
		execv(argv[0], argv);
		err(-1, "execv");
	default:
		break;
	}

	free(argv);

	return (0);
}