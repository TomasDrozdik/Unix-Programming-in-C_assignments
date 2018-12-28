/* argument-parser.c */

// TODO valgrind still reports some memory leaks.

/*
 *  - Process a file from argv[1] to build argv for execv(2).  You must not
 *   use a statically allocated `*argv` array for pointers as the number of
 *   arguments may be unlimited.  There is one argument per line in the file.
 *   You may only process the file once, i.e. you must not first read the
 *   file into memory, then count the lines, allocate argv, then process the
 *   file again to populate the argv.  You may not use mmap(2) even if you
 *   know how to.
 *   - Hint: use macros from <queue.h> to build a linked list of arguments
 *   while reading the file, then allocate and populate argv using the linked
 *   list.  *
 *   - Example #1:
 * ```
 * $ cat args
 * /bin/echo
 * hello
 * world  *
 * $ ./a.out args
 * hello world
 * ```
 *   - Example #2:
 * ```
 *   $ cat args
 *   /usr/bin/touch
 *   foo
 *   bar
 *   ahem ahem  *
 *   $ ./a.out
 *   $ ls -1
 *   foo
 *   bar
 *   ahem ahem
 * ```
 */

#define _POSIX_C_SOURCE 200809L

#define DEBUG

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <unistd.h>

struct entry
{
	char *arg;

	SLIST_ENTRY(entry) entries;
};

typedef SLIST_HEAD(slisthead, entry) slist_t;

void
parse_file(FILE *fp, int *argc, slist_t *head)
{
	ssize_t readlen;
	char *lineptr = NULL;
	size_t n = 0;
	struct entry *newentryp;

	SLIST_INIT(head);
	*argc = 0;

	errno = 0;
	while ((readlen = getline(&lineptr, &n, fp)) != -1) {
		if (errno != 0) {
			err(-1, "getline");
		}

		// If we get empty line don't do anything
		if (readlen == 1 && *lineptr == '\n') {
			free(lineptr);
			continue;
		}

		newentryp = malloc(sizeof (newentryp));
		newentryp->arg = lineptr;

		// Strip the \n at the end
		lineptr[readlen - 1] = 0;

		SLIST_INSERT_HEAD(head, newentryp, entries);

		++(*argc);
		// Make the lineptr NULL so that getline initializes new char*
		lineptr = NULL;
	}
}

void
exec_args(int argc, slist_t *head)
{
	// Allocate size of argv + 1 for NULL required by execv
	char **argv = malloc(argc * sizeof (char *) + 1);

	// Populate the array in reverse order with respect to slist
	int i = argc;
	struct entry *ent;
	SLIST_FOREACH(ent, head, entries) {
		argv[--i] = ent->arg;
	}

	// Add NULL ptr required by execv
	argv[argc] = NULL;

#ifdef DEBUG
	assert (i == 0);
	for (i = 0; i < argc; ++i) {
		printf("%s\n", argv[i]);
	}
#endif

	switch (fork()) {
	case -1:
		err(-1, "fork");
	case 0:
		execv(argv[0], argv);
		err(-1, "execv");
		break;
	}

	free(argv);
}

void
free_slist(slist_t *head)
{
	struct entry *ent;
#ifdef DEBUG
	int i = 0;
#endif

	while (!SLIST_EMPTY(head)) {
#ifdef DEBUG
		++i;
#endif
		ent = SLIST_FIRST(head);
		free(ent->arg);
		SLIST_REMOVE_HEAD(head, entries);
		free(ent);
	}

#ifdef DEBUG
		printf("Number of unalocated strings: %d\n", i);
#endif
}

int
main(int argc, char *argv[])
{
	int argcount;
	FILE *fp;
	slist_t args = SLIST_HEAD_INITIALIZER(head);

	if (argc < 2) {
		err(-1, "usage: ./a.out <file>");
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		err(-1, "open");
	}


	parse_file(fp, &argcount, &args);
	exec_args(argcount, &args);

	free_slist(&args);

	if (fclose(fp) == EOF) {
		err(-1, "open");
	}

	return (0);
}