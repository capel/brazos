#ifndef MALLOC_H
#define MALLOC_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

#include "types.h"

int mem_init(size_t pages);
void* malloc(size_t size);
void* calloc(size_t size, size_t obj_size);

int free(void *ptr);
void* realloc(void *ptr, size_t newsize);

void Mem_Dump();


#endif 
