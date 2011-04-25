#include "disk.h"
#include "kfs.h"
#include "stdlib.h"
#include "kio.h"

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

#define DISK_STATUS_FAILURE 0


inline static void set_read_addr(disk_addr addr) {
    *((volatile unsigned*)(DEV_DISK_ADDRESS + DEV_DISK_OFFSET_HIGH32)) = 0;
    *((volatile unsigned*)(DEV_DISK_ADDRESS +DEV_DISK_OFFSET)) = addr;
}

inline static unsigned start_operation(int op) {
    *((volatile unsigned*)(DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION)) = op;
    return *((volatile unsigned*)(DEV_DISK_ADDRESS + DEV_DISK_STATUS));
}

inline static bool rw_internal_block(size_t intern_addr, char* buf, int op) {
    set_read_addr(intern_addr);
    size_t status = start_operation(op);
    if (status != DISK_STATUS_FAILURE) {
        memcpy(buf, (volatile char*)DEV_DISK_BUFFER, DEV_DISK_BUFFER_LEN);
    }
    return (bool)status;
}

static bool rw_block(disk_addr addr, char* buf, int op) {
    size_t intern_addr = addr * 4;
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
    return rw_block(addr, buf, DEV_DISK_OPERATION_READ);
}
bool kwrite_block(disk_addr addr, const char* buf) {
    return rw_block(addr, buf, DEV_DISK_OPERATION_WRITE);
}

inode_t kdisk_get_free_inode(void) {
    return 0xdeadbeef;

}

// Also marks the block as taken
disk_addr kfind_free_block(void);
void kfree_block(disk_addr addr);


