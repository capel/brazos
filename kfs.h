#ifndef KFS_H
#define KFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif


#include "types.h"
#include "disk.h"

#define KFS_DIR 1
#define KFS_NORMAL_FILE 2

#define FILES_PER_DIR 8
#define FILENAME_LEN 32


struct _kfile;

typedef struct _dir_entry {
    char name[FILENAME_LEN];
    inode_t inode;
    size_t type;
} dir_entry;


typedef size_t (*write_func)(struct _kfile* f, const char* buf, 
        size_t len, size_t pos);
typedef size_t (*read_func)(struct _kfile* f, char* buf, 
        size_t len, size_t pos);
typedef void (*delete_func)(struct _kfile* f);
typedef int (*flush_func)(struct _kfile* f);
typedef dir_entry* (*get_dir_entries_func)(struct _kfile* dir);
typedef void (*put_dir_entries_func)(struct _kfile* dir, bool dirty);
typedef bool (*add_to_dir_func)(struct _kfile *dir, 
        const char * name, struct _kfile* add);
typedef bool (*rm_from_dir_func)(struct _kfile* dir, struct _kfile *rm);


typedef struct _kfile {
    inode_t inode;
    int type;
    size_t size;
    size_t ref_count;
    size_t link_count;
    const char* dir_name; // dir only!
    void* private_data;

    // all files
    flush_func flush;
    delete_func delete_self;
    
    // Normal file funcs
    write_func write;
    read_func read;

    // Directory funcs
    get_dir_entries_func get_entries;
    put_dir_entries_func put_entries;
    add_to_dir_func add_file;
    rm_from_dir_func rm_file;
    
    disk_addr dblocks[NUM_DBLOCKS];
} kfile;
void ksetup_fs(void);

// NULL if filename already exists.
kfile* kf_create(int type);

void print_dir(kfile* dir);

kfile* kf_lookup(const char* name, kfile* start);

/*
size_t kf_write(kfile* f, const char* buf, size_t len, size_t pos);
size_t kf_read(kfile* f, char* buf, size_t len, size_t pos);
void kf_delete(kfile* f);

bool kf_add_to_dir(const char * name, kfile* f, kfile *dir);
bool kf_rm_from_dir(kfile* file, kfile *dir);
*/

const char* kf_get_name(kfile* f, kfile *dir);
bool kf_copy_dir_entries(kfile* dir, void* space, size_t size);

kfile* kget_file(inode_t inode);
void kput_file(kfile* f);

void kf_setup_dir(kfile* f);
void kf_setup_new_dir(kfile* f);
void kf_setup_normal_file(kfile *f);
void kf_setup_new_normal_file(kfile *f);


kinode* kget_inode(inode_t inode);
void kput_inode(const kinode* inode, bool dirty);

kfile* root(void);

// This can always be called from any function
// pointer type because it immediately panics.
void vfs_bad_func();

void kfs_register_file(kfile* f);

#endif
