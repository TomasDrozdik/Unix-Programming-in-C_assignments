#include <stdio.h>

int
main(void)
{
    void *p;
    p = &p;

    printf("%zu -> %zu", p,*((unsigned long long *) p));
    
    return 0;
}    
