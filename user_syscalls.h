#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"
#include "syscalls.h"
#include "stdio.h"

void perror(int error);

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

static inline void exec(char* name) {
    syscall(EXEC, (int)name, 0, 0);
}

static inline void yield() {
    syscall(YIELD, 0, 0, 0);
}

static inline int forkexec(char* name) {
    int ret = syscall(FORKEXEC, (int)name, 0, 0);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }
    return ret;
}

static inline void wait(int pid) {
    syscall(WAIT, pid, 0, 0);
}

static inline int open(const char* name) {
    int ret = syscall(OPEN, (int)name, 0, 0);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }
    return ret;
}

static inline int write(int fd, const char* buf, size_t len) {
    int ret = syscall(WRITE, fd, (int)buf, len);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }
    return ret;
}

static inline int read(int fd, char* buf, size_t len) {
    int ret = syscall(READ, fd, (int)buf, len);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }
    return ret;
}

static inline int seek(int fd, int offset, int type) {
    int ret = syscall(SEEK, fd, offset, type);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }
    return ret;
}

static inline int create(const char* name, int flags) {
    int ret = syscall(CREATE, (int)name, flags, 0);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }   
    return ret;
}

static inline int link(int prid, int crid, const char* name) {
    int ret = syscall(SYS_LINK, prid, crid, (int)name);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }   
    return ret;
}

static inline int close(int fd) {
    int ret = syscall(CLOSE, fd, 0, 0);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }   
    return ret;
}

static inline int unlink(const char* path) {
    int ret = syscall(SYS_UNLINK, (int) path, 0, 0);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }   
    return ret;
}

static inline int set_cwd(const char* path) {
    return syscall(SET_CWD, (int)path, 0, 0);
}

static inline int get_cwd(char* name_space, size_t size) {
    return syscall(GET_CWD, (int)name_space, size, 0);
}

static inline void get_dir_entries(void* space, size_t size) {
    syscall(GET_DIR_ENTRIES, (int)space, size, 0);
}

static inline int lookup(const char* path) {
  int ret = syscall(SYS_LOOKUP, (int)path,0, 0);
  if (ret < 0) {
    perror(ret);
  }
  return ret;
}

static inline int map(int rid, size_t *out_size, void** out_ptr) {
  int ret = syscall(SYS_MAP, rid, (int)out_size, (int)out_ptr);
  if (ret < 0) {
    perror(ret);
  }
  return ret;
}

#endif
