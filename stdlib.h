#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"


// Vararg handling, stdarg.h style, let GCC handle the nastiness..
typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

void memset(char* start, char fill, size_t bytes);
void memcpy(char* dst, const char * src, size_t len);
int memcmp(const char* a, const char* b, size_t bytes);

size_t strlen(const char* str);
size_t strlcpy(char* dst, const char* src, size_t len);
int strcmp(const char* a, const char* b, size_t len);

// allocates a new string long enough to hold a + b + NULL.
// this new string does not overlap with a or b in memory, it must
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

void itoa(char* buf, size_t size, int s);
void utoa(char* buf, size_t size, unsigned s);
void utoa16(char* buf, size_t size, unsigned x);


#endif
