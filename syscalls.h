#ifndef SYSCALLS_H
#define SYSCALLS_H

#define READ_STDIN 1
#define WRITE_STDOUT 2
#define HALT 3
#define GET_PAGE 4
#define FREE_PAGE 5

int syscall(int r0, int r1, int r2, int r3);

#endif
