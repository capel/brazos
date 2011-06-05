#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#define EXTERNAL
#include "disk.h"
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
  
    for(SIZE_T u = 1; u< 4096 * 50; u++) {
        write(f, &u, sizeof(u));
    }
    
    
    close(f);
    return 0;
}
