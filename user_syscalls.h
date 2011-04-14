#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"
#include "syscalls.h"

static inline int getc() {
    return syscall(GETC, 0, 0, 0);
}

static inline int putc(int c) {
    return syscall(PUTC, c, 0, 0);
}

static inline void* get_pages(size_t num) {
    return (void*)syscall(GET_PAGES, num, 0, 0);
}

static inline void halt() {
    syscall(HALT, 0, 0, 0);
}

static inline void _exit() {
    syscall(EXIT, 0, 0, 0);
}

static inline void yield() {
    syscall(YIELD, 0, 0, 0);
}

#endif
