#include<stdio.h>
#include<stdlib.h>

int totalspacealloc = 0;

void* myalloc(int n)
{
    void *a = malloc(n + sizeof(int));
    totalspacealloc += n;
    *(int*) a = n;
    return (a + sizeof(int));
}

void myfree(void *ptr)
{
    totalspacealloc -= *(int*)(ptr-sizeof(int));
    free(ptr-sizeof(int));
}
