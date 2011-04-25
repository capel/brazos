#ifndef BCACHE_H
#define BCACHE_H

#ifdef USER
#error __FILE__ " included from a user context"
#endif

#include "types.h"

typedef struct _kblock {
    void* maddr;
    disk_addr daddr;
    bool dirty;
    size_t ref_count;
} kblock;

void* kget_block(disk_addr block);
void kput_block(disk_addr block, bool dirty);
void kflush_block(disk_addr block);

void kbcache_init(void);

#endif
