#ifndef SYSCALLS_H
#define SYSCALLS_H

#define READ_STDIN 1
#define WRITE_STDOUT 2
#define HALT 3
#define GET_PAGES 4
#define FREE_PAGES 5

int syscall(int r0, int r1, int r2, int r3);

static inline void gets(char* buf, size_t size) {
    syscall(READ_STDIN, (int)buf, size, 0);
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

#endif
