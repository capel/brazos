#include "mem.h"
#include "stdlib.h"
#include "kio.h"

void* mstart;
size_t msize;
size_t mpages;
char *bitmap;
size_t mcurrent;


const char space[] =  {
8 , 7 , 6 , 6 , 5 , 5 , 5 , 5 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 3 , 3 , 
3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 5 , 4 , 3 , 3 , 2 , 2 , 
2 , 2 , 3 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 4 , 3 , 2 , 2 , 2 , 2 , 2 , 2 , 3 , 
2 , 2 , 2 , 2 , 2 , 2 , 2 , 6 , 5 , 4 , 4 , 3 , 3 , 3 , 3 , 3 , 2 , 2 , 2 , 
2 , 2 , 2 , 2 , 4 , 3 , 2 , 2 , 2 , 1 , 1 , 1 , 3 , 2 , 1 , 1 , 2 , 1 , 1 , 
1 , 5 , 4 , 3 , 3 , 2 , 2 , 2 , 2 , 3 , 2 , 1 , 1 , 2 , 1 , 1 , 1 , 4 , 3 , 
2 , 2 , 2 , 1 , 1 , 1 , 3 , 2 , 1 , 1 , 2 , 1 , 1 , 1 , 7 , 6 , 5 , 5 , 4 , 
4 , 4 , 4 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 4 , 3 , 2 , 2 , 2 , 2 , 2 , 2 , 
3 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 5 , 4 , 3 , 3 , 2 , 2 , 2 , 2 , 3 , 2 , 1 , 
1 , 2 , 1 , 1 , 1 , 4 , 3 , 2 , 2 , 2 , 1 , 1 , 1 , 3 , 2 , 1 , 1 , 2 , 1 , 
1 , 1 , 6 , 5 , 4 , 4 , 3 , 3 , 3 , 3 , 3 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 4 , 
3 , 2 , 2 , 2 , 1 , 1 , 1 , 3 , 2 , 1 , 1 , 2 , 1 , 1 , 1 , 5 , 4 , 3 , 3 , 
2 , 2 , 2 , 2 , 3 , 2 , 1 , 1 , 2 , 1 , 1 , 1 , 4 , 3 , 2 , 2 , 2 , 1 , 1 , 
1 , 3 , 2 , 1 , 1 , 2 , 1 , 1 , 0 };

const char start_pos[] = {
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 4 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 3 , 3 , 3 , 3 , 0 , 0 , 
0 , 0 , 5 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 4 , 4 , 0 , 0 , 0 , 0 , 0 , 0 , 5 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 5 , 2 , 2 , 2 , 
2 , 2 , 2 , 2 , 4 , 4 , 4 , 4 , 6 , 0 , 0 , 0 , 5 , 5 , 0 , 0 , 6 , 0 , 0 , 
0 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 5 , 5 , 0 , 0 , 6 , 0 , 0 , 0 , 4 , 4 , 
4 , 4 , 6 , 0 , 0 , 0 , 5 , 5 , 0 , 0 , 6 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 4 , 4 , 1 , 1 , 1 , 1 , 1 , 1 , 
5 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 5 , 5 , 1 , 
1 , 6 , 1 , 1 , 1 , 4 , 4 , 4 , 4 , 6 , 1 , 1 , 1 , 5 , 5 , 1 , 1 , 6 , 1 , 
1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 5 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 4 , 
4 , 4 , 4 , 6 , 2 , 2 , 2 , 5 , 5 , 2 , 2 , 6 , 2 , 2 , 2 , 3 , 3 , 3 , 3 , 
3 , 3 , 3 , 3 , 5 , 5 , 3 , 3 , 6 , 3 , 3 , 3 , 4 , 4 , 4 , 4 , 6 , 4 , 4 , 
4 , 5 , 5 , 5 , 5 , 6 , 6 , 7 , 0 };
const char beginning_space[] =  { 
8 , 7 , 6 , 6 , 5 , 5 , 5 , 5 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 4 , 3 , 3 , 3 , 
3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 2 , 2 , 2 , 2 , 2 , 2 , 
2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 
2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 };



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

static void mark_page_used(size_t page)
{
    size_t pos = page / 8; // find the byte
    size_t bit = page & 8;
    bitmap[pos] |= (1 << bit); // set bit
    printk("Marking: byte: %x (page %u)", bitmap[pos], page);
}

static void mark_page_free(size_t page)
{
    size_t pos = page / 8; // find the byte
    size_t bit = page & 8;
    bitmap[pos] &= ~(1 << bit); // clear bit
}

void ksetup_memory(void)
{
    printk("Setup memory ==>");
    setup_mem_bounds();
    
    mcurrent = 0;

    /*
    // we are limiting ourselves to ~134MBs right now,
    // but that's a reasonable tradeoff for simplicity
    // here we are taking the first page for our free-page bitmap
    bitmap = get_addr_from_page(0);
    memset(bitmap, 0, PAGE_SIZE);
    mark_page_used(0);
    */
    printk("<== Done with memory setup");
}

void* kget_page()
{
    return kget_pages(1);
}

void* kget_pages(size_t num)
{

    /*
    if (num > 8) return 0; // handle larger allocs later

    size_t start_page;

    for (size_t i = 0; i < mpages / 8; i++) {
        if (space[(int)bitmap[i]] >= num) {
                space[(int)bitmap[i]], bitmap[i]);
            size_t start = start_pos[(int)bitmap[i]];
            start_page = i * 8 + start;
            for(size_t j = num; j != 0 ; j--) {
                mark_page_used(start++);
            }
            break;
        }
    }
    
    */
    
    size_t start_page = mcurrent;
    mcurrent += num;
    printk("Allocing %u pages at %p (page %u)", 
        num, get_addr_from_page(start_page), start_page);
    
    *(char*)get_addr_from_page(start_page) = 0; // poke to make sure memory is actually there
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
    //size_t page = get_page_from_addr(pageaddr);
    printk("Free %u pages at %p (page %u)", 
        num, pageaddr, get_page_from_addr(pageaddr));

    /*
    for(size_t i = 0; i < num; i++)
    {
        mark_page_free(page+i);
    }*/
}
