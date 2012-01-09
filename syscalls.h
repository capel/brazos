#ifndef SYSCALLS_H
#define SYSCALLS_H

#define HALT 20
#define EXIT 21

#define EXEC 22
#define FORKEXEC 23

#define SYS_VIEW 1

// directory ops
#define SYS_LOOKUP 2
#define SYS_LINK 3
#define SYS_UNLINK 4

// sinkhole operations
#define SYS_SINK 5
#define SYS_MESSAGE 6
#define SYS_DREDGE 7
#define SYS_WAIT 8

#define SYS_TYPE 9

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
