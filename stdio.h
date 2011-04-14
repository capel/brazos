#ifndef STDIO_H
#define STDIO_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"
#include "chars.h"

#define debug(x, args...) println(GREEN "db: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x, __LINE__, __func__, ## args)


int snprintf(char* buf, size_t size, const char* fmt, ...);
int printf(const char* fmt, ...);
int println(const char* fmt, ...);

int getline(char* buf, size_t size);

#endif
