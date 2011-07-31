#include "../common/malloc.h"
#include "malloc.h"
#include "stdio.h"
#include "lock.h"

malloc_data user_md;
const alloc_funcs user_alloc_funcs = {malloc, calloc, realloc, free, printf};

void* get_pages() {
  return 0;
}

int mem_init(size_t pages)
{
    int r;
    user_md.get_pages = get_pages;
    user_md.lock = LOCK_INIT;
    user_md.db = println;
    lock(&user_md.lock);
    r = _mem_init(pages, &user_md);
    unlock(&user_md.lock);
    return r;

}

// user versions of malloc
void* malloc(size_t size)
{
    void* r;
    lock(&user_md.lock);
    r = _malloc(size, &user_md);
    unlock(&user_md.lock);
    return r;
}

void* realloc(void* ptr, size_t newsize)
{
    void* r;
    lock(&user_md.lock);
    r = _realloc(ptr, newsize, &user_md);
    unlock(&user_md.lock);
    return r;
}


void* calloc(size_t size, size_t objsize)
{
    void *r;
    lock(&user_md.lock);
    r = _calloc(size, objsize, &user_md);
    unlock(&user_md.lock);
    return r;
}

int free(void* ptr)
{
    int r;
    lock(&user_md.lock);
    r = _free(ptr, &user_md);
    unlock(&user_md.lock);
    return r;
}


