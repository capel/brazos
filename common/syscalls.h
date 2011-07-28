#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "types.h"

#define SYS_WRITE_STDOUT 3
#define SYS_PUTC 4
#define SYS_GETC 5

#define SYS_YIELD 6

#define SYS_LOOKUP 7
#define SYS_LINK 8
#define SYS_LINK_MSG 9
#define SYS_UNLINK 10
#define SYS_MAP 11
#define SYS_UNMAP 12
#define SYS_LIST 13
// Range 0 < err_t < 4096
typedef size_t err_t;
// Range 4096 <= rid_t < SIZE_T_MAX
typedef size_t rid_t;
// * P_R Read permission.
// * P_COW Can read and write, but writing creates a new copy.
// * P_W Can read from and write to the original.
// Note that permissions are strictly increasing in size as
// they become more permissive, so they can be compared like
// integers.
typedef size_t perms_t;

// Permission defines
#define P_R 0xf001
#define P_COW 0xf002
#define P_W 0xf003


#endif
