#ifndef USER_H_H
#define USER_H_H

#include "types.h"

int sh_main(void);
int bc_main(void);

void exit();
typedef bool (*readline_func)(char*);
void readline_lib(const char * prompt, readline_func func); 

#endif
