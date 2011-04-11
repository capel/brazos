#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"
#include "syscalls.h"

static inline char getc() {
    return (char) syscall(GETC, 0, 0, 0);
}

static inline size_t gets(char* buf, size_t size) {
    return syscall(READ_STDIN, (int)buf, size, 0);
}

static inline void* get_pages(size_t num) {
    return (void*)syscall(GET_PAGES, num, 0, 0);
}

static inline void free_pages(void* page) {
    syscall(FREE_PAGES, (int)page, 0, 0);
}

static inline void halt() {
    syscall(HALT, 0, 0, 0);
}

static inline void _exit() {
    syscall(EXIT, 0, 0, 0);
}

#endif
