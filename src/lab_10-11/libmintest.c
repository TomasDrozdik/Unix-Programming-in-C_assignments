/* limmintest.c */

#include <stdio.h>
#include <unistd.h>

extern int min(int a[], ssize_t len);

int
main(void)
{
    int a[] = { 0, 1, 2, 3, 4 };

    int len = sizeof(a) / sizeof(a[0]);

    printf("%d\n", min(a, len));
}
