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
#define LINK 17
#define UNLINK 18

#define SET_CWD 19
#define GET_CWD 20
#define GET_DIR_ENTRIES 21

// Not syscalls; flags for various syscalls
#define SEEK_ABS 1
#define SEEK_REL 2

#define CREATE_DIR 1
#define CREATE_FILE 2

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
