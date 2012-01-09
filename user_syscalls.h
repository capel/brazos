#ifndef USER_SYSCALL_H
#define USER_SYSCALL_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"
#include "syscalls.h"
#include "stdio.h"

void perror(int error);

static inline void halt() {
    syscall(HALT, 0, 0, 0);
}

static inline void _exit() {
    syscall(EXIT, 0, 0, 0);
}

static inline void exec(char* name) {
    syscall(EXEC, (int)name, 0, 0);
}

static inline int forkexec(char* name) {
    int ret = syscall(FORKEXEC, (int)name, 0, 0);
    if (ret < 0) {
        debug("Error code: %d", ret);
    }
    return ret;
}

static inline int wait(int rid) {
  int ret = syscall(SYS_WAIT, rid, 0, 0);
  if (ret < 0) {
    perror(ret);
  }
  return ret;

}

// wait 0 is defined to do nothing at all
// so basicly just a yield
static inline void yield() {
  wait(0);
}


static inline int link(int prid, int crid, const char* name) {
    int ret = syscall(SYS_LINK, prid, crid, (int)name);
    if (ret < 0) {
      perror(ret);
    }   
    return ret;
}

static inline int unlink(int rid, const char* path) {
    int ret = syscall(SYS_UNLINK, rid, (int) path, 0);
    if (ret < 0) {
      perror(ret);
    }   
    return ret;
}

static inline int lookup(const char* path) {
  int ret = syscall(SYS_LOOKUP, (int)path,0, 0);
  if (ret < 0) {
    perror(ret);
  }
  return ret;
}

static inline int view(int rid, char* buf, size_t len) {
  int ret = syscall(SYS_VIEW, rid, (int)buf, len);
  if (ret != 0) {
    perror(E_ERROR);
  }
  return ret;
}

static inline int message(void* ptr, size_t size) {
  int ret = syscall(SYS_MESSAGE, (int)ptr, (int)size, 0);
  if (ret < 0) {
    perror(ret);
  }
  return ret;
}

static inline int sink(int src_rid, int sh_rid) {
  int ret = syscall(SYS_SINK, src_rid, sh_rid, 0);
  if (ret < 0) {
    perror(ret);
  }
  return ret;
}

static inline int type(int rid) {
  int ret = syscall(SYS_TYPE, rid, 0, 0);
  if (ret <0) {
    perror(ret);
  }
  return ret;
}

static inline int dredge(int rid) {
  int ret = syscall(SYS_DREDGE, rid, 0, 0);
  if (ret < 0) {
    perror(ret);
  }
  return ret;
}


#endif
