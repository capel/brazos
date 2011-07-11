#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"

typedef void* (*malloc_func)(size_t);
typedef void* (*calloc_func)(size_t,size_t);
typedef void* (*realloc_func)(void*,size_t);
typedef int (*free_func)(void*);

typedef void* (*get_pages_func)(size_t);

// this is an odd place, but it's useful
typedef int (*printf_func)(const char*, ...);

typedef struct {
    malloc_func malloc;
    calloc_func calloc;
    realloc_func realloc;
    free_func free;
    printf_func printf;
} alloc_funcs;

typedef struct {
    struct free_node_t* head;
    size_t size;
    lock_t lock;
    get_pages_func get_pages;
    printf_func db;
} malloc_data;

int _mem_init(size_t pages, malloc_data *md);
void* _malloc(size_t size, malloc_data *md);
void* _calloc(size_t size, size_t obj_size, malloc_data *md);
void* _realloc(void *ptr, size_t newsize, malloc_data *md);
int _free(void *ptr, malloc_data *md);

void Mem_Dump(malloc_data *md);

#endif 
