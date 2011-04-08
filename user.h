#ifndef USER_H
#define USER_H

#include "syscalls.h"
#include "stdlib.h"
#include "kio.h"

int syscall(int r0, int r1, int r2, int r3);

void gets(char* buf, size_t size) {
    syscall(READ_STDIN, (int)buf, size, 0);
}

void main()
{
    char buf[500];
    printf("Hello\n");
    char * page = syscall(GET_PAGE, 0xdead, 0xbeef, 0xbabe);
    printf("Page %p\n", page);
    for (;;)
    {
        gets(buf, 500);
        printf(buf);
    }
}

void exit()
{
    printf("Death comes for us all\n");
    syscall(HALT, 0, 0, 0);
}

#endif
