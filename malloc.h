#ifndef MALLOC_H
#define MALLOC_H

#ifdef KERNEL
#error __FILE__ " included from kernel code."
#endif

int mem_init(size_t pages);
void* malloc(int size);
int free(void *ptr);
void Mem_Dump();


#endif 
