#ifndef MEM_H
#define MEM_H

#include "mach.h"

#define PAGE_SIZE 4096

void setup_memory(void);
void* get_page(void);
void* get_pages(size_t num);

void free_page(void* pageaddr);
void free_pages(void* pageaddr, size_t num);

#endif
