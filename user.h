#ifndef USER_H
#define USER_H

#include "syscalls.h"
#include "stdlib.h"
#include "kio.h"

void main()
{
    printf("!!!HelloA\n");
    char * page = get_pages(1); 
    printf("Page %p\n", page);
    for (;;) {
        gets(page, 4096);
        printf("%s", page);
    }
}

void main2()
{
    printf("!!!HelloB\n");
    char * page = get_pages(1); 
    printf("Page %p\n", page);
    for (;;) {
        printf("B");
    }
}

void exit(void) {
    _exit();
}

#endif
