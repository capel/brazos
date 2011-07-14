#include "../common/malloc.h"
#include "include/ent_gen.h"
#include "include/mach.h"

void* mstart;
size_t msize;
size_t mpages;
char *bitmap;
size_t mcurrent;

malloc_data kmalloc_data;
alloc_funcs kernel_alloc_funcs = {
  kmalloc, 
  kcalloc, 
  krealloc, 
  kfree, 
  _printk
};


void* kmalloc(size_t size) {
    return _malloc(size, &kmalloc_data);
}
void* kcalloc(size_t size, size_t obj_size) {
    return _calloc(size, obj_size, &kmalloc_data);
}
void* krealloc(void *ptr, size_t newsize) {
    return _realloc(ptr, newsize, &kmalloc_data);
}
int kfree(const void *ptr) {
    return _free((void*)ptr, &kmalloc_data);
}
static void setup_mem_bounds(void)
{
    size_t kend = (size_t) &_kend;

    mstart = (void*) ( (kend / PAGE_SIZE)*PAGE_SIZE + PAGE_SIZE);
    msize = get_mem_size() - (size_t)&mstart;
    mpages = msize / PAGE_SIZE;    
}

void* kget_addr_from_page(size_t page)
{
    return mstart + PAGE_SIZE * page;
}

size_t kalloc_raw_pages(size_t num) {
  size_t start_page = mcurrent;
  mcurrent += num;
  return start_page;
}

static void* get_raw_pages(size_t num) {
  size_t pages = kalloc_raw_pages(num);
  return kget_addr_from_page(pages);
}

void ksetup_memory(void)
{
    setup_mem_bounds();
    kmalloc_data.get_pages = get_raw_pages;
    kmalloc_data.db = _printk;
    _mem_init(10, &kmalloc_data); 
    mcurrent = 0;
}

