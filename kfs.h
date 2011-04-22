#ifndef KFS_H
#define KFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif

#include "types.h"
#include "disk.h"

#define KFS_DIR 1
#define KFS_FILE 2


typedef struct _kfile {
    size_t inode;
    kinode* inode_ptr;
    size_t size;
    int type;
    char * data;
    size_t ref_count;
    const char* dir_name; // dir only!
    void* blocks[NUM_DBLOCKS];
} kfile;

// NULL if filename already exists.
kfile* kf_create(int type);

kfile* kf_lookup(const char* name, kfile* start);
size_t kf_write(kfile* f, char* buf, size_t len, size_t pos);
size_t kf_read(kfile* f, char* buf, size_t len, size_t pos);

bool kf_add_to_dir(const char * name, kfile* f, kfile *dir);
bool kf_rm_from_dir(kfile* file, kfile *dir);

bool kf_get_dir_entries(kfile* dir, void* space, size_t size);

const char* kf_get_name(kfile* f, kfile *dir);

kfile* kf_get_root(void);

void ksetup_fs(void);

int kflush_dirty_file(kfile* f);

#endif
