#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define EXTERNAL
#include "disk.h"
#include "syscalls.h"
int f;

void fillzero(int num) {
    char buf[1];
    buf[0] = 0;
    for(int i = 0; i < num; i++) {
        int err = write(f, buf, 1);
        if (err < 0) perror(NULL);
    }
}


int main(int argc, char** argv) {
    f = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY);
    printf("file: %s, %d", argv[1], f);
   
    // block 0
    fillzero(BLOCK_SIZE);
    // superblock();
    fillzero(BLOCK_SIZE);

    // inode map
    char buf[1];
    buf[0] = 1;
    write(f, buf, 1); 
    write(f, buf, 1); 
    fillzero(BLOCK_SIZE - 2);

    // block map
    write(f, buf, 1); 
    write(f, buf, 1); 
    fillzero(BLOCK_SIZE - 2);


    fillzero(sizeof(kinode));
    kinode t;
    kinode * pt = &t;

    pt->inode = 1;
    pt->size = 320;
    pt->flags = CREATE_DIR; // kfs dir
    pt->link_count = 1337;
    pt->dblocks[0] = 1; // the one we alloced


    write(f, pt, sizeof(kinode));
    fillzero(BLOCK_SIZE - sizeof(kinode));
    fillzero(BLOCK_SIZE * 50);
    close(f);
    return 0;
}
