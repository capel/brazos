#include "mem.h"
#include "stdlib.h"
#include "kio.h"

void* mstart;
size_t msize;
size_t mpages;
char *bitmap;
size_t mcurrent;



static void setup_mem_bounds(void)
{
    size_t kend = (size_t) &_kend;

    mstart = (void*) ( (kend / PAGE_SIZE)*PAGE_SIZE + PAGE_SIZE);
    msize = get_mem_size() - (size_t)&mstart;
    mpages = msize / PAGE_SIZE;    


    printk("Kernel %p - %p\n Memory %p for %u (%u pages, %u bytes total)", 
            &_kstart, &_kend, mstart, msize, mpages, get_mem_size());

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
    printk("Allocing %u pages at %p (page %u)", 
        num, get_addr_from_page(start_page), start_page);
    
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
    printk("Free %u pages at %p (page %u)", 
        num, pageaddr, get_page_from_addr(pageaddr));
}
