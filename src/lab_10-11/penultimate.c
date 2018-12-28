/* penultimate.c */
/*
 * Program to print only the penultimate line from STDIN to STDOUT.
 */

#define _GNU_SOURCE     // for getline
#define BUFF_SIZE 1024

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    size_t ret;
    size_t len = BUFF_SIZE * sizeof(char);
    char *prev_line = malloc(len);
    char *next_line = malloc(len);
    char *penultimate;

    ret = getline(&prev_line, &len, stdin);
    assert(ret != -1);

    ret = getline(&next_line, &len, stdin);
    assert(ret != -1);

    penultimate = prev_line;

    while (getline(&next_line, &len, stdin) != -1) {
        penultimate = prev_line;
        prev_line = next_line;
        next_line = penultimate; 
    }
    
    printf("%s", penultimate);
    
    free(prev_line);
    free(next_line);
    
    return 0;
}
