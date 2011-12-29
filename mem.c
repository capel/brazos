#include "mem.h"
#include "stdlib.h"
#include "kio.h"
#include "malloc.h"


void* mstart;
size_t msize;
size_t mpages;
char *bitmap;
size_t mcurrent;

void* kmalloc(size_t size) {
    return malloc(size);
}
void* kcalloc(size_t size, size_t obj_size) {
    void* p = malloc(size * obj_size);
    memset(p, 0, size* obj_size);
    return p;
}
void kfree(void *ptr) {
    free(ptr);
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

void kfree_page(void* pageaddr)
{
    kfree_pages(pageaddr, 1);
}

void kfree_pages(void* pageaddr, size_t num)
{
    // make it obvious we have free this.
    memset(pageaddr, 0xff, PAGE_SIZE * num);
}
