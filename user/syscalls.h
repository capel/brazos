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

static inline rid_t lookup(const char * path) {
  return syscall(SYS_LOOKUP, (int)path, 0, 0);
}

static inline err_t link(const char *path, rid_t child) {
  return syscall(SYS_LINK, (int)path, (int) child, 0);
}

static inline err_t link_msg(const char *path, const char* buf, size_t size) {
  return syscall(SYS_LINK_MSG, (int)path, (int)buf, (int)size);
}

static inline err_t unlink(const char *path) {
  return syscall(SYS_UNLINK, (int)path, 0, 0);
}

static inline err_t map(rid_t resource, perms_t requested_perms, 
    size_t *out_size, void **out_ptr) {
  return syscall(SYS_MAP, resource, requested_perms, 0);
}

static inline err_t unmap(rid_t resource) {
  return syscall(SYS_UNMAP, resource, 0, 0);
}


#endif
