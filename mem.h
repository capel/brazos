#ifndef MEM_H
#define MEM_H

#ifdef USER
#error __FILE__ " included from user code."
#endif

#include "mach.h"

#define PAGE_SIZE 4096

void ksetup_memory(void);
void* kget_page(void);
void* kget_pages(size_t num);

void kfree_page(void* pageaddr);
void kfree_pages(void* pageaddr, size_t num);

void* kmalloc(size_t size);
void* kcalloc(size_t size, size_t obj_size);
void kfree(void *ptr);

void reset_kernel_vm(void);

#endif
