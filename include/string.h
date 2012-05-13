#ifndef STRING_H
#define STRING_H


void* memset(void* start, int fill, size_t bytes);
void* memcpy(void* dst, const void * src, size_t len);
int memcmp(const char* a, const char* b, size_t bytes);


size_t strlen(const char* str);
size_t strlcpy(char* dst, const char* src, size_t len);
int strncmp(const char* a, const char* b, size_t len);
int strcmp(const char* a, const char* b);

#endif
