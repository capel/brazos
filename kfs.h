#ifndef KFS_H
#define KFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif


#include "types.h"
#include "disk.h"
#include "kvector.h"

#define KFS_DIR 1
#define KFS_NORMAL_FILE 2

#define FILES_PER_DIR 8
#define FILENAME_LEN 32

#define DIR_ENTRIES_SIZE (FILES_PER_DIR * sizeof(dir_entry))


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
typedef void (*cleanup_func)(struct _kfile* f);
typedef int (*flush_func)(struct _kfile* f);
typedef dir_entry* (*get_dir_entries_func)(struct _kfile* dir);
typedef void (*put_dir_entries_func)(struct _kfile* dir, bool dirty);
typedef bool (*add_to_dir_func)(struct _kfile *dir, 
        const char * name, struct _kfile* add);
typedef bool (*rm_from_dir_func)(struct _kfile* dir, struct _kfile *rm);
typedef struct _kfile* (*lookup_file_func)(struct _kfile* dir, 
        vector * v, size_t level);
typedef bool (*set_parent_dir_func)(struct _kfile* f, struct _kfile* parent);
typedef int (*added_to_dir_func)(struct _kfile* f, struct _kfile* dir);
typedef int (*removed_from_dir_func)(struct _kfile* f, struct _kfile* dir);

typedef struct _file_funcs {
    // Directory funcs
    get_dir_entries_func get_entries;
    put_dir_entries_func put_entries;
    add_to_dir_func add_file;
    rm_from_dir_func rm_file;
    lookup_file_func lookup_file;
    
    // Status change callbacks
    added_to_dir_func added;
    removed_from_dir_func removed;

    // flush
    flush_func flush;
    cleaup_func cleanup;

    // read/write
    write_func write;
    read_func read;
} file_funcs;

typedef struct _kfile {
    size_t ref_count;
    void* d;
    void* f;
} kfile;

typedef struct _normal_file_data {
    inode_t inode;
    size_t size;
    size_t link_count;
    disk_addr dblocks[NUM_DBLOCKS];
} normal_file_data;

typedef _normal_dir_data {
    inode_t inode;
    size_t link_count;
    const char* name;
    disk_addr data;
} noramL_dir_data;

#define kfile_get(f) ((f)->ref_count++)
void kfile_put(kfile* f);

void ksetup_fs(void);

kfile* kf_create_normal_file(void);
kfile* kf_create_normal_dir(void);

kfile* kf_lookup_name(const char* name, kfile* start);
kfile* kf_lookup_inode(inode_t inode);

const char* kf_get_name(kfile* f, kfile *dir);
bool kf_copy_dir_entries(kfile* dir, void* space, size_t size);

void kf_setup_dir(kfile* f);
void kf_setup_new_dir(kfile* f);
void kf_setup_new_normal_file(kfile *f);

kinode* kget_inode(inode_t inode);
void kput_inode(const kinode* inode, bool dirty);

kfile* root(void);
kfile* kalloc_file(void);

void kfs_register_file(kfile* f);

const char* kitoa(int i);
const char* kstrclone(const char* s);

#endif
