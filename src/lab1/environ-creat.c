/* environ-creat.c */
/*
  - environment variable manipulation
    - write a program that will create a set of new environment variables based on a static string of the form "XXXX=Y"
      where it will create env vars "XXXX", "XXX", "XX", "X" - all with the value of "Y"
      - use pointer arithmetics and putenv() for that, i.e. no memory will be allocated
      - print environment (see environ(7)) before and after the manipulation (with a separator)
*/

#define _GNU_SOURCE   // for getenv


#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void
usage(char *name)
{
    printf("Usage: %s arg1=val1 arg2=val2...\n", basename(name));
}

int
process_arg(char *str)
{
    // Locate '=' in str
    char *eq = strchr(str, '=');
    
    // Assert that arg and val is non empty and that '=' is present
    if (eq <= str || strlen(eq) < 1) {
        return -1;
    }

    char *it = str;
    while (it < eq) {
        putenv(it++);
    }

    return 0;
}

extern char **environ;

int
main(int argc, char *argv[])
{
    /* Print enviroment variables */ 
    for (int i = 0; environ[i] != NULL; ++i) {
        printf("%s\n", environ[i]);
    }

    /* Print separator. */
    printf("\n");
    for (int i = 0; i < 40; ++i)
        printf("=");
    printf("\n\n");

    /* Add new enviroment variables. */
    for (int i = 1; i < argc; ++i) {
       if (process_arg(argv[i]) == -1) {
           usage(argv[0]);
           return 1;
       }
    }

    /* Print enviroment variables */ 
    for (int i = 0; environ[i] != NULL; ++i) {
        printf("%s\n", environ[i]);
    }

    return 0;
}
