typedef size_t err_t;

#define SUCCESS 1
#define E_NOT_SUPPORTED 2
#define E_BAD_FILENAME 3
#define E_BAD_PROGRAM 4
#define E_IS_DIR 5
#define E_BAD_FD 6
#define E_ISNT_DIR 7
#define E_BAD_ARG 8
#define E_BAD_SYSCALL 9
#define E_NOT_FOUND 10
#define E_ALREADY_EXISTS 11
#define E_ERROR 12

#define MAX_ERROR 32

#define IS_ERROR(x) (((err_t)(x)) < MAX_ERROR)
