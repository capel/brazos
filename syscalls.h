#ifndef SYSCALLS_H
#define SYSCALLS_H

#define PUTC 1
#define WRITE_STDOUT 2
#define HALT 3
#define GET_PAGES 4
#define EXIT 6
#define GETC 7
#define YIELD 8
#define EXEC 9

int syscall(int r0, int r1, int r2, int r3);


#endif
