#ifndef DISK_H
#define DISK_H

#ifdef USER
#error __FILE__ " included from a user context"
#endif

#include "types.h"

#define	DEV_DISK_ADDRESS		0x13000000
#define	    DEV_DISK_OFFSET		    0x0000
#define	    DEV_DISK_OFFSET_HIGH32	    0x0008
#define	    DEV_DISK_ID			    0x0010
#define	    DEV_DISK_START_OPERATION	    0x0020
#define	    DEV_DISK_STATUS		    0x0030
#define	    DEV_DISK_BUFFER		    0x4000

#define	    DEV_DISK_BUFFER_LEN		0x200

/*  Operations:  */
#define	DEV_DISK_OPERATION_READ		0
#define	DEV_DISK_OPERATION_WRITE	1


#define NUM_DBLOCKS 13
#define BLOCK_SIZE 512

typedef size_t disk_addr;

typedef struct _kinode {
    size_t size;
    size_t flags;
    size_t ref_count;
    disk_addr dblocks[NUM_DBLOCKS]
} kinode;

int kread_block(disk_addr addr, char* buf);
int kwrite_block(disk_addr addr, const char* buf);

kinode* kread_inode(size_t inode);
void kwrite_inode(size_t inode, kinode* inode);

// Also marks the block as taken
disk_addr kfind_free_block(void);
void kfree_block(disk_addr addr);

#endif
