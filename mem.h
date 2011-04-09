#ifndef MEM_H
#define MEM_H

#include "mach.h"

#define PAGE_SIZE 4096

void ksetup_memory(void);
void* kget_page(void);
void* kget_pages(size_t num);

void kfree_page(void* pageaddr);
void kfree_pages(void* pageaddr, size_t num);

#endif
