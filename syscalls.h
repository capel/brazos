#ifndef SYSCALLS_H
#define SYSCALLS_H

#define HALT 1
#define EXIT 2

#define PUTC 4
#define GETC 5

#define YIELD 6
#define EXEC 7
#define FORKEXEC 8
#define WAIT 9

#define SYS_VIEW 12

// directory ops
#define SYS_LOOKUP 24
#define SYS_LINK 17
#define SYS_UNLINK 18

// sinkhole operations
#define SYS_SINK 25
#define SYS_MESSAGE 26
#define SYS_DREDGE 27

#define SYS_TYPE 28

// types
#define KO_UNKNOWN 'U'
#define KO_MESSAGE 'M'
#define KO_DIR 'D'
#define KO_SINKHOLE 'S'
#define KO_FOUNTAIN 'N'
#define KO_FUTURE 'F'
#define KO_ERROR 'E'


#define SINK_ASYNC 0

#define E_NOT_SUPPORTED -1
#define E_ERROR -2
#define E_BAD_FILENAME -3
#define E_BAD_PROGRAM -4
#define E_BAD_RID -5
#define E_NOT_DIR -6
#define E_BAD_ARG -7
#define E_BAD_SYSCALL -8
#define E_NOT_SINKHOLE -9
#define E_IS_FUTURE -10
#define E_NOT_FOUND -11
#define E_NOT_FOUNTAIN -12

#ifndef EXTERNAL
int syscall(int r0, int r1, int r2, int r3);
#endif

#endif
