#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"
#include "chars.h"

#include "malloc.h"
#include <string.h>

#define PAGE_SIZE 4096

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) > (b)) ? (b) : (a)

// Vararg handling, stdarg.h style, let GCC handle the nastiness..
typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

void itoa(char* buf, size_t size, int s);
void utoa(char* buf, size_t size, unsigned s);
void utoa16(char* buf, size_t size, unsigned x);

int atoi(const char* buf);

int power(int a, int b);

#ifdef USER
#define assert(x) do { if (!(x)) { debug("Assert (%s) in %s at " __FILE__ ":%d failed.", #x, __func__, __LINE__); _exit(); } } while(0)
#else
#include "../kio.h"
#define assert(x) do { if (!(x)) { panic("Assert (%s) in %s at " __FILE__ ":%d failed.", #x, __func__, __LINE__); } } while(0)
#endif

#endif
