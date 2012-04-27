#ifndef USER_H
#define USER_H

#define USER

#include "user_syscalls.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"

void main()
{
    mem_init(10);
    printf("!!!HelloA\n");
    char* page = malloc(100);
    
    printf("Page %p\n", page);
    for (;;) {
        gets(page, 100);
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
