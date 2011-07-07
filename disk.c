#include "disk.h"
#include "kfs.h"
#include "stdlib.h"
#include "kio.h"
#include "bcache.h"


#define	DEV_DISK_ADDRESS		0x13000000
volatile unsigned* DEV_DISK_OFFSET = (void*) (DEV_DISK_ADDRESS + 0);
volatile unsigned* DEV_DISK_ID = (void*)( DEV_DISK_ADDRESS + 0x10);
volatile unsigned* DEV_DISK_START_OP = (void*)(DEV_DISK_ADDRESS + 0x20);
volatile unsigned* DEV_DISK_STATUS = (void*)(DEV_DISK_ADDRESS + 0x30);
volatile unsigned char* DEV_DISK_BUFFER = (void*)(DEV_DISK_ADDRESS + 0x4000);

#define	    DEV_DISK_BUFFER_LEN		0x200

/*  Operations:  */
#define	DEV_DISK_OPERATION_READ		0
#define	DEV_DISK_OPERATION_WRITE	1

#define DISK_STATUS_FAILURE 0

static char* inode_map;
static char* block_map;

void ksetup_disk() {
    block_map = kget_block(BLOCK_MAP);
    inode_map = kget_block(INODE_MAP);
}

inline static void set_read_addr(disk_addr addr) {
    *DEV_DISK_OFFSET = addr * INTERNAL_DISK_BLOCK_SIZE; 
}

inline static bool start_operation(int op) {
    *DEV_DISK_ID = 0;
    *DEV_DISK_START_OP = op;
    
    
    assert(0 != *DEV_DISK_STATUS);
    return true;
}

static bool rw_internal_block(size_t intern_addr, char* buf, int op) {
    set_read_addr(intern_addr);
  //  printk("addr: %x buf %p op %d, len %d", intern_addr*INTERNAL_DISK_BLOCK_SIZE, buf, op, DEV_DISK_BUFFER_LEN);
    start_operation(op);
    if (op == DEV_DISK_OPERATION_READ) {
        for(int i = 0; i < DEV_DISK_BUFFER_LEN; i++) {
            buf[i] = DEV_DISK_BUFFER[i];
        }
    } else {
        for(int i = 0; i < DEV_DISK_BUFFER_LEN; i++) {
            DEV_DISK_BUFFER[i] = buf[i];
        }
    }
  //  printk("Done with memcpy(%d, %p)", intern_addr, buf);
    return true; //start_operation(op);
}

void print_dblock(char *buf) {
    for (int i = 0; i < DEV_DISK_BUFFER_LEN / sizeof(unsigned); i += 8) {
        unsigned* ib = (unsigned*)buf;
        printk("%d : %u %u %u %u %u %u %u %u", i*sizeof(unsigned), ib[i+0], 
                ib[i+1], ib[i+2], ib[i+3], ib[i+4], ib[i+5], ib[i+6], ib[i+7]); 
    }
}

static bool rw_block(disk_addr addr, char* buf, int op) {
    size_t intern_addr = addr * 8;
    // we'll and together to find a failure. 
    // Unless they are all true, the result is false.
    bool status = true;

    status &= rw_internal_block(intern_addr, buf, op);
    status &= rw_internal_block(intern_addr + 1, buf + DEV_DISK_BUFFER_LEN*1, op);
    status &= rw_internal_block(intern_addr + 2, buf + DEV_DISK_BUFFER_LEN*2, op);
    status &= rw_internal_block(intern_addr + 3, buf + DEV_DISK_BUFFER_LEN*3, op);
   

    return status;
}

bool kread_block(disk_addr addr, char* buf) {
    bool r=  rw_block(addr, buf, DEV_DISK_OPERATION_READ);
        return r;
}
bool kwrite_block(disk_addr addr, char* buf) {
    return rw_block(addr, buf, DEV_DISK_OPERATION_WRITE);
}

static void free_bit(char* map, disk_addr block) {
    map[block] = 0;
}

// Also marks the block as taken
static size_t find_bit(char* map) {
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        if (map[i] == 0) {
            map[i] = 1;
        //    printk("allocing %d", i);
            assert(map[i] == 1);
            return i;
        }
    }
    return 0;
    printk("no free space");
}


inode_t kalloc_inode(void) {
 //   printk("Alloc inode");
    inode_t i = find_bit(inode_map);
    assert(i != 0);
    assert(inode_map[i] == 1);
    return i;
}

void kfree_inode(inode_t i) {
    free_bit(inode_map, i);
}

disk_addr kalloc_block(void) {
 //   printk("Alloc block");
    disk_addr d = BLOCK_START + find_bit(block_map);
    assert(d != 0);
    return d;
}

void kfree_block(disk_addr block) {
    free_bit(block_map, block);
}


