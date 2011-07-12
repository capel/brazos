#ifndef MEM_H
#define MEM_H

#include "ent.h"

#define PAGE_SIZE 4096

typedef ent pages;
typedef ent mm;

ent* kcreate_mm(void);
void ksetup_memory(void);

void* kmalloc(size_t size);
void* kcalloc(size_t size, size_t obj_size);
void* krealloc(void *ptr, size_t newsize);
int kfree(const void *ptr);


#endif
