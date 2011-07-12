#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "../common/types.h"
#include "../common/syscalls.h"

int syscall(int r0, int r1, int r2, int r3);

static inline int getc() {
    return syscall(SYS_GETC, 0, 0, 0);
}

static inline int putc(int c) {
    return syscall(SYS_PUTC, c, 0, 0);
}

static inline void yield() {
    syscall(SYS_YIELD, 0, 0, 0);
}

static inline void write_stdout(const char* c) {
  syscall(SYS_WRITE_STDOUT, (int)c, 0, 0);
}

#endif
