#ifndef STDIO_H
#define STDIO_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"

int snprintf(char* buf, size_t size, const char* fmt, ...);
int printf(const char* fmt, ...);
int println(const char* fmt, ...);

void getline(char* buf, size_t size);

#endif
