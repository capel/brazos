#include "mem.h"
#include "stdlib.h"
#include "kio.h"
#include "malloc.h"


void* mstart;
size_t msize;
size_t mpages;
char *bitmap;
size_t mcurrent;

malloc_data kmalloc_data;

void* kmalloc(size_t size) {
    return _malloc(size, &kmalloc_data);
}
void* kcalloc(size_t size, size_t obj_size) {
    return _calloc(size, obj_size, &kmalloc_data);
}
void* krealloc(void *ptr, size_t newsize) {
    return _realloc(ptr, newsize, &kmalloc_data);
}
int kfree(void *ptr) {
    return _free(ptr, &kmalloc_data);
}
static void setup_mem_bounds(void)
{
    size_t kend = (size_t) &_kend;

    mstart = (void*) ( (kend / PAGE_SIZE)*PAGE_SIZE + PAGE_SIZE);
    msize = get_mem_size() - (size_t)&mstart;
    mpages = msize / PAGE_SIZE;    
}

static void* get_addr_from_page(size_t page)
{
    return mstart + PAGE_SIZE * page;
}

void ksetup_memory(void)
{
    setup_mem_bounds();
    kmalloc_data.get_pages = kget_pages;
    _mem_init(10, &kmalloc_data); 
    mcurrent = 0;
}

void* kget_page()
{
    return kget_pages(1);
}

void* kget_pages(size_t num)
{
    size_t start_page = mcurrent;
    mcurrent += num;
  //  printk("Allocing %u pages at %p (page %u)", 
  //      num, get_addr_from_page(start_page), start_page);
    
    return get_addr_from_page(start_page);
}

static size_t get_page_from_addr(void* addr)
{
    size_t relative = addr - mstart;
    return (size_t)relative / PAGE_SIZE;
}

void kfree_page(void* pageaddr)
{
    kfree_pages(pageaddr, 1);
}

void kfree_pages(void* pageaddr, size_t num)
{
  //  printk("Free %u pages at %p (page %u)", 
  //      num, pageaddr, get_page_from_addr(pageaddr));
}
