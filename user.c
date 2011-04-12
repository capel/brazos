#ifndef USER_H
#define USER_H

#define USER

#include "user_syscalls.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include "vector.h"

void main()
{
    mem_init(1);
    printf("HelloA\n");
    char* page = calloc(100, 1);

    for (;;) {
        getline(page, 100);
        vector * v = split_to_vector(page, " ");
        print_vector(v, "%s|");
        println("");
        cleanup_vector(v);
    }
}

void main2()
{
    printf("!!!HelloB\n");
    char * page = get_pages(1); 
    printf("Page %p\n", page);
    for (;;) { printf("!"); }
}

void exit(void) {
    _exit();
}

#endif
