#ifndef KTYPES_H
#define KTYPES_H

#ifndef EXTERNAL

#include <stddef.h>
 
typedef unsigned uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef unsigned char byte;

//typedef unsigned size_t;
#define true 1
#define false 0
//#define NULL 0
#define SIZE_T size_t
#else
#include <stdint.h>
#define SIZE_T uint32_t
#endif
typedef int bool;
typedef SIZE_T disk_addr;
typedef SIZE_T inode_t;

typedef SIZE_T lock_t;
#define LOCK_INIT 0


#endif
