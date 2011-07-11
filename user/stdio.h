#ifndef STDIO_H
#define STDIO_H

#include "../common/types.h"
#include "../common/chars.h"

#define debug(x, args...) println(GREEN "db: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)


int snprintf(char* buf, size_t size, const char* fmt, ...);
int printf(const char* fmt, ...);
int println(const char* fmt, ...);

int getline(char* buf, size_t size);
typedef bool (*readline_func)(char*);
void readline_lib(const char * prompt, readline_func func); 

#endif
