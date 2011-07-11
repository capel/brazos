#ifndef USER_MALLOC_H
#define USER_MALLOC_H
#include "../common/malloc.h"

int mem_init(size_t pages);
void* malloc(size_t size);
void* calloc(size_t size, size_t obj_size);
void* realloc(void *ptr, size_t newsize);
int free(void *ptr);

extern const alloc_funcs user_alloc_funcs;

#endif
