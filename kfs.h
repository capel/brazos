#ifndef KFS_H
#define KFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif


#include "types.h"
#include "disk.h"

#define KFS_DIR 1
#define KFS_FILE 2

#define ROOT_INODE 1

typedef struct _kfile {
    inode_t inode;
    int type;
    size_t size;
    size_t ref_count;
    size_t link_count;
    const char* dir_name; // dir only!
    disk_addr dblocks[NUM_DBLOCKS];
} kfile;

void ksetup_fs(void);

// NULL if filename already exists.
kfile* kf_create(int type);

kfile* kf_lookup(const char* name, kfile* start);
size_t kf_write(kfile* f, const char* buf, size_t len, size_t pos);
size_t kf_read(kfile* f, char* buf, size_t len, size_t pos);
void kf_delete(kfile* f);

bool kf_add_to_dir(const char * name, kfile* f, kfile *dir);
bool kf_rm_from_dir(kfile* file, kfile *dir);
void kf_mkdir(kfile* f);
bool kf_get_dir_entries(kfile* dir, void* space, size_t size);

const char* kf_get_name(kfile* f, kfile *dir);

kfile* kget_file(inode_t inode);
void kput_file(kfile* f);


int kflush_file(kfile* f);

kinode* kget_inode(inode_t inode);
void kput_inode(const kinode* inode, bool dirty);





#endif
