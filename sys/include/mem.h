#ifndef MEM_H
#define MEM_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "mach.h"
#include "common.h"
#include "ent.h"

#define PAGE_SIZE 4096

typedef ent pages;

void ksetup_memory(void);
pages* kget_page(void);
pages* kget_pages(size_t num);

void* kmalloc(size_t size);
void* kcalloc(size_t size, size_t obj_size);
void* krealloc(void *ptr, size_t newsize);
int kfree(void *ptr);


#endif
