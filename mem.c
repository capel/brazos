#include "mem.h"
#include "stdlib.h"
#include "kio.h"

void* mstart;
size_t msize;
size_t mpages;
char *bitmap;

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



void setup_mem_bounds(void)
{
    size_t kend = (size_t) &_kend;

    mstart = (void*) ( (kend / PAGE_SIZE)*PAGE_SIZE + PAGE_SIZE);
    msize = get_mem_size() - (size_t)&mstart;
    mpages = msize / PAGE_SIZE;    


    kprintf("Kernel %p - %p\n Memory %p for %u (%u pages, %u bytes total)\n", 
            &_kstart, &_kend, mstart, msize, mpages, get_mem_size());

}

void* get_addr_from_page(size_t page)
{
    return mstart + PAGE_SIZE * page;
}

void mark_page_used(size_t page)
{
    size_t pos = page / 8; // find the byte
    size_t bit = page & 8;
    bitmap[pos] |= (1 << bit); // set bit
    kprintf("Marking: byte: %x Byte (page %u)\n", bitmap[pos], page);
}

void mark_page_free(size_t page)
{
    size_t pos = page / 8; // find the byte
    size_t bit = page & 8;
    bitmap[pos] &= ~(1 << bit); // clear bit
}

void setup_memory(void)
{
    kprintf("Setup memory\n");
    setup_mem_bounds();
    
    // we are limiting ourselves to ~134MBs right now,
    // but that's a reasonable tradeoff for simplicity
    // here we are taking the first page for our free-page bitmap
    bitmap = get_addr_from_page(0);
    memset(bitmap, 0, PAGE_SIZE);
    mark_page_used(0);

    kprintf("Done mem setup\n");
}

void* get_page()
{
    return get_pages(1);
}

void* get_pages(size_t num)
{

    if (num > 8) return 0; // handle larger allocs later

    size_t start_page;

    for (size_t i = 0; i < mpages / 8; i++) {
        if (space[(int)bitmap[i]] >= num) {
            kprintf("Test page: space: %u, byte %x\n", 
                space[(int)bitmap[i]], bitmap[i]);
            size_t start = start_pos[(int)bitmap[i]];
            kprintf("mem start: %u\n", start);
            start_page = i * 8 + start;
            for(size_t j = num; j != 0 ; j--) {
                mark_page_used(start++);
            }
            break;
        }
    }
    kprintf("Allocing %u pages at %p (page %u)\n", 
        num, get_addr_from_page(start_page), start_page);
    return get_addr_from_page(start_page);
}

size_t get_page_from_addr(void* addr)
{
    size_t relative = addr - mstart;
    return (size_t)relative / PAGE_SIZE;
}

void free_page(void* pageaddr)
{
    free_pages(pageaddr, 1);
}

void free_pages(void* pageaddr, size_t num)
{
    size_t page = get_page_from_addr(pageaddr);
    kprintf("Free %u pages at %p (page %u)\n", 
        num, pageaddr, get_page_from_addr(pageaddr));
    for(size_t i = 0; i < num; i++)
    {
        mark_page_free(page+i);
    }
}
