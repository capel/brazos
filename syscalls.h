#ifndef SYSCALLS_H
#define SYSCALLS_H


#define HALT 1
#define EXIT 2

#define GET_PAGES 3

#define PUTC 4
#define GETC 5

#define YIELD 6
#define EXEC 7
#define FORKEXEC 8
#define WAIT 9

#define WRITE 10
#define WRITE_STDOUT 11
#define READ 12
#define SEEK 13
#define OPEN 14
#define CREATE 15
#define CLOSE 16

#define SET_CWD 19
#define GET_CWD 20
#define GET_DIR_ENTRIES 21


#define SYS_MAP 22
#define SYS_UNMAP 23
#define SYS_LOOKUP 24
#define SYS_LINK 17
#define SYS_UNLINK 18

// Not syscalls; flags for various syscalls
#define SEEK_ABS 1
#define SEEK_REL 2

#define CREATE_DIR 1
#define CREATE_FILE 2

#define E_NOT_SUPPORTED -2
#define E_ERROR -1
#define E_BAD_FILENAME -3
#define E_BAD_PROGRAM -4
#define E_IS_DIR -5
#define E_BAD_FD -6
#define E_ISNT_DIR -7
#define E_BAD_ARG -8
#define E_BAD_SYSCALL -9

#ifndef EXTERNAL
int syscall(int r0, int r1, int r2, int r3);
#endif

extern size_t kdir_entries_space;

#define FILES_PER_DIR 8
#define FILENAME_LEN 32

typedef struct  {
    char name[FILENAME_LEN];
    inode_t inode;
    size_t type;
} user_dir_entry;
#define GET_DIR_ENTRIES_SPACE FILES_PER_DIR * sizeof(user_dir_entry)

#endif
