#include "bcache.h"
#include "mem.h"
#include "hashmap.h"
#include "disk.h"

#define KBCACHE_SIZE 3

static hashmap* cache;
static size_t active_pages;
static size_t max_pages;

extern alloc_funcs kernel_alloc_funcs;

void kbcache_init() {
    cache = make_hashmap(KBCACHE_SIZE, &kernel_alloc_funcs);
    active_pages = 0;
    max_pages = 1 << KBCACHE_SIZE;
}


// if 0, evict something random
static void evict_block() {
    kblock* b = hm_delete_random(cache);

    if (b->dirty) {
        kwrite_block(b->daddr, b->maddr);   
        kfree_page(b->maddr);
        kfree(b);
    }
}

static kblock* make_block(disk_addr block) {
    kblock* b = kmalloc(sizeof(*b));
    b->maddr = kget_page();
    b->daddr = block;
    kread_block(block, b->maddr);
    b->dirty = false;
    b->ref_count = 0;
    return b;
}

static void add_block(kblock* b) {
    active_pages++;
    if (active_pages >= max_pages) {
        evict_block(0);
    }
    hm_insert(cache, b->daddr, b);
}

void* kget_block(disk_addr block) {
    assert(block != 0);

    kblock * b = hm_lookup(cache, block);
    if (b) {
        b->ref_count++;
        return b->maddr;
    } else {
        kblock * b = make_block(block);
        add_block(b);
        b->ref_count++;
        return b;
    }
}

void kflush_block(disk_addr block) {
    assert(block != 0);

    kblock * b = hm_lookup(cache, block);
    if (b && b->dirty) {
        kwrite_block(b->daddr, b->maddr);   
        b->dirty = false;
    }
}

void kput_block(disk_addr block, bool dirty) {
    assert(block != 0);

    kblock * b = hm_lookup(cache, block);
    assert(b);
    b->dirty = true ? dirty || b->dirty == true : false;
    b->ref_count--;
}

