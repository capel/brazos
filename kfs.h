#ifndef KFS_H
#define KFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif


#include "types.h"
#include "disk.h"
#include "kvector.h"
#include "kobj.h"

#define KFS_DIR 1
#define KFS_NORMAL_FILE 2

#define FILES_PER_DIR 8
#define FILENAME_LEN 32

typedef ko kfile;
typedef ko kdir;

void ksetup_fs(void);

kfile* kcreate_file(void);
kdir* kcreate_dir(void);

kfile* kget_by_path(const char* name, kdir* start);
kfile* kget_by_inode(inode_t inode);

kdir* root(void);

#endif
