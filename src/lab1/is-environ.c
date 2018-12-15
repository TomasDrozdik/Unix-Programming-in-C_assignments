/* is-environ.c */
/*
    - Program will get a list of arguments that are environment variable names.
    - The program will recognize 2 options:
        -a      will check that all variables are defined. If yes return 0,
                otherwise return 1.
        -o      will check that at least one variable is defined.
                If yes return 0, otherwise return 1.

        Examples:

        ./main -a SHELL HOME
        0
        ./main -o SHELL NONEXISTENT
        0
        ./main SHELL HOME PWD
        - should print usage
 */

#define _XOPEN_SOURCE   // for getopt

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void
usage(char *name)
{
    printf("Usage: %s [-ao] [--] arg1 arg2...\n", basename(name));
}

extern char *optarg;
extern int optind, opterr, optopt;

int
main(int argc, char *argv[])
{
    int opt;
    char *options = "ao";

    opt = getopt(argc, argv, options);
    
    switch (opt) {
    case 'a':
        for (int i = optind; i < argc; ++i) {
            if (getenv(argv[i]) == NULL)
                return 1;
        }
        return 0;

    case 'o':
        for (int i = optind; i < argc; ++i) {
            if (getenv(argv[i]) != NULL)
                return 0;
        }
        return 1;
    
    default:
        usage(argv[0]);
        return -1;
    }

    assert(0);
    return -1;
}
