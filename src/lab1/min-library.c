/* min-library.c */
/*
truct a library
    - it will implement function
      int min(int a[], ssize_t len);
    - and a program that will link against the library and call the min() function with sample inputs
    - write separate Makefiles for the program and the shared library
    - make sure runtime path is set correctly
*/

#include <limits.h>
#include <unistd.h>

int
min(int a[], ssize_t len)
{
    int min = INT_MIN;
    for (ssize_t i = 0; i < len; ++i) {
       if (min > a[i]) 
           min = a[i];
    }

    return min;
}
