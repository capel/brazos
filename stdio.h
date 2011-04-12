#ifndef STDIO_H
#define STDIO_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"

#define BLACK "\033[22;30m"
#define RED "\033[22;31m"
#define GREEN "\033[22;32m"
#define BROWN "\033[22;33m" 
#define BLUE "\033[22;34m"
#define MAGENTA "\033[22;35m" 
#define CYAN "\033[22;36m"
#define GRAY "\033[22;37m"
#define DARK_GRAY "\033[01;30m"
#define LIGHT_RED "\033[01;31m"
#define LIGHT_GREEN "\033[01;32m" 
#define YELLOW "\033[01;33m"
#define LIGHT_BLUE "\033[01;34m"
#define LIGHT_MAGENTA "\033[01;35m" 
#define LIGHT_CYAN "\033[01;36m"
#define WHITE "\033[01;37m" 

#define debug(x, args...) println(GREEN "db: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)


int snprintf(char* buf, size_t size, const char* fmt, ...);
int printf(const char* fmt, ...);
int println(const char* fmt, ...);

void getline(char* buf, size_t size);

#endif
