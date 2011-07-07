#ifndef DISK_H
#define DISK_H

#ifdef USER
#error __FILE__ " included from a user context"
#endif

#include "types.h"

#define NUM_DBLOCKS 12
#define INTERNAL_DISK_BLOCK_SIZE 512
#define BLOCK_SIZE 4096
 

#define SUPERBLOCK 1
#define INODE_MAP 2
#define BLOCK_MAP 3
#define INODE_START 4
#define BLOCK_START 10

#define INODES_PER_BLOCK (BLOCK_SIZE / sizeof(kinode))

#define PAGE_SIZE 4096

#define VIRTUAL_BLOCK 0xBADF000D
#define VIRTUAL_INODE 0

typedef struct _kinode {
    inode_t inode;
    SIZE_T size;
    SIZE_T flags;
    SIZE_T link_count;
    disk_addr dblocks[NUM_DBLOCKS];
} kinode;

void ksetup_disk(void);

bool kread_block(disk_addr addr, char* buf);
bool kwrite_block(disk_addr addr, char* buf);


// Also marks the block as taken
disk_addr kalloc_block(void);
void kfree_block(disk_addr addr);
inode_t kalloc_inode(void);
void kfree_inode(inode_t i);

static inline void zero_page(void* page) {
    unsigned *s = page;
    for (size_t i = 0; i < PAGE_SIZE / sizeof(unsigned); i++) {
        s[i] = 0;
    }
}


#endif
