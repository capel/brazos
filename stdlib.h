#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"
#include "chars.h"

#define PAGE_SIZE 4096

// Vararg handling, stdarg.h style, let GCC handle the nastiness..
typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

void memset(void* start, char fill, size_t bytes);
void memcpy(void* dst, const void * src, size_t len);
int memcmp(const void* a, const void* b, size_t bytes);

size_t strlen(const char* str);
size_t strlcpy(char* dst, const char* src, size_t len);
int strncmp(const char* a, const char* b, size_t len);
int strcmp(const char* a, const char* b);

// allocates a new string long enough to hold a + b + NULL.
// this new string does not overlap with a or b in memory, and it must
// be freed seperately.
char* strgcat(const char* a, const char* b, size_t alen, size_t blen);

/*
void* malloc(size_t bytes);
void free(void* addr);
*/

bool isspace(char c);
bool isupper(char c);
bool islower(char c);
bool isalpha(char c);
bool isdigit(char c);

bool isdigits(const char* c);


void itoa(char* buf, size_t size, int s);
void utoa(char* buf, size_t size, unsigned s);
void utoa16(char* buf, size_t size, unsigned x);

int atoi(const char* buf);

int power(int a, int b);

bool lock(lock_t* l);
bool unlock(lock_t* l);

#ifdef USER
#define assert(x) do { if (!(x)) { debug("Assert (%s) in %s at " __FILE__ ":%d failed.", #x, __func__, __LINE__); _exit(); } } while(0)
#else
#include "kio.h"
#define assert(x) do { if (!(x)) { panic("Assert (%s) in %s at " __FILE__ ":%d failed.", #x, __func__, __LINE__); } } while(0)
#endif

#endif
