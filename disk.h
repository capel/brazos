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
#define FREE_BLOCK_MAP 3
#define INODE_BLOCK 4

#define INODES_PER_BLOCK (BLOCK_SIZE / sizeof(kinode))

typedef struct _kinode {
    inode_t inode;
    size_t size;
    size_t flags;
    size_t link_count;
    disk_addr dblocks[NUM_DBLOCKS];
} kinode;

bool kread_block(disk_addr addr, char* buf);
bool kwrite_block(disk_addr addr, const char* buf);


// Also marks the block as taken
disk_addr kfind_free_block(void);
void kfree_block(disk_addr addr);
inode_t kdisk_get_free_inode(void);

#endif
