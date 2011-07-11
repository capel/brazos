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
alloc_funcs kernel_alloc_funcs = {kmalloc, kcalloc, krealloc, kfree, _printk};

ONLY_CLEANUP(pages_funcs, cleanup_pages);

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

static void* get_raw_pages(size_t num) {
    size_t start_page = mcurrent;
    mcurrent += num;
    
    return get_addr_from_page(start_page);
}

void ksetup_memory(void)
{
    setup_mem_bounds();
    kmalloc_data.get_pages = get_raw_pages;
    kmalloc_data.db = _printk;
    _mem_init(10, &kmalloc_data); 
    mcurrent = 0;
}

pages* kget_page()
{
    return kget_pages(1);
}

pages* kget_pages(size_t num)
{
    size_t start_page = mcurrent;
    mcurrent += num;
    
    pages* p = entalloc(pages_funcs);
    p->d2 = num;
    p->d1 = get_addr_from_page(start_page);
    return p;
}

static size_t get_page_from_addr(void* addr)
{
    size_t relative = addr - mstart;
    return (size_t)relative / PAGE_SIZE;
}

void kfree_pages(pages* p)
{
    // make it obvious we have freed this.
    memset(p->d, 0xff, PAGE_SIZE * p->d2);
}
