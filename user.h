#ifndef USER_H
#define USER_H

#include "syscalls.h"
#include "stdlib.h"
#include "kio.h"

void main()
{
    char buf[500];
    printf("Hello\n");
    char * page = get_pages(1); 
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
    halt();

}

#endif
