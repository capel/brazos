#include "stdlib.h"
#include "kfs.h"
#include "mem.h"
#include "kvector.h"
#include "bcache.h"

#define FILES_PER_DIR 8
#define FILENAME_LEN 32

typedef struct _dir_entry {
    char name[FILENAME_LEN];
    inode_t inode;
    size_t type;
} dir_entry;

#define GET_DIR_DATA(dir) ((dir_entry*)(kget_block((dir)->dblocks[0])))
#define PUT_DIR_DATA(dir, dirty) (kput_block((dir)->dblocks[0], dirty))


void print_dir(kfile* dir) {
    assert(dir);
    printk("DIR: %s (%d:%p)", dir->dir_name, dir->inode, dir);
    printk("dir block %d", dir->dblocks[0]);
    if (dir->dblocks[0]) {
        dir_entry* de = GET_DIR_DATA(dir);
        printk("dir block addr %p", de);
        for (size_t i = 0; i < FILES_PER_DIR; i++) {
            if (de[i].inode) {
                printk("* %s (%d) : %d", de[i].name, de[i].inode, de[i].type);
            }
        }
        PUT_DIR_DATA(dir, false);
    } else {
        printk("UNALLOCED dir_block. WTF?!");
    }

}


static inode_t dir_lookup(vector* v, size_t level, inode_t i);

void kf_mkdir(kfile* f) {
    f->size = FILES_PER_DIR * sizeof(dir_entry);
    f->dir_name = 0;
    f->dblocks[0] = kalloc_block();
    void * b = kget_block(f->dblocks[0]);
    memset(b, 0, PAGE_SIZE);
    kput_block(f->dblocks[0], true);
}

bool kf_add_to_dir(const char * name, kfile* f, kfile *dir) {
    dir_entry* de = GET_DIR_DATA(dir); 
    assert(de);

    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].inode == 0) {
            strlcpy(de[i].name, name, FILENAME_LEN);
            de[i].inode = f->inode;
            de[i].type = f->type;
            f->link_count++;
            printk("Adding %s (%d) to %s (%d)", name, 
                f->inode, dir->dir_name, dir->inode);
            if (f->type == KFS_DIR && f->dir_name == 0) {
                f->dir_name = de[i].name;
            }

            kflush_file(dir);
            PUT_DIR_DATA(dir, true);
            return true;
        }
    }
    printk("No space left in dir %s (%d)", f->dir_name, f->inode);
    print_dir(f);
    PUT_DIR_DATA(dir, false);
    return false;
}

bool kf_rm_from_dir(kfile* f, kfile *dir) {
    dir_entry* de = GET_DIR_DATA(dir); 
    
    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].inode == f->inode) {
            de[i].inode = 0;
            PUT_DIR_DATA(dir, true);
            f->link_count--;
            return true;
        }
    }
    PUT_DIR_DATA(dir, false);
    return false;
}


bool kf_get_dir_entries(kfile *dir, void* space, size_t size) {
    
    assert(size >= FILES_PER_DIR * sizeof(dir_entry)); 
    assert(dir->type == KFS_DIR);
    printk("size: %d dir->size %d", size, dir->size);
    assert(size >= dir->size);

    dir_entry* de = GET_DIR_DATA(dir); 

    memcpy(space, (char*) de, dir->size);
    PUT_DIR_DATA(dir, false);
    return true;
}

kfile* kf_lookup(const char* name, kfile *start) {
    assert(start);
    assert(start->type == KFS_DIR);
    assert(start->inode);
    
    bool abs = false;

    if (0 == strcmp(name, "/")) {
        return root();
    }

    if (name[0] == '/') {
        // root dir
        start = root();
        name++;
    }

    vector *v = ksplit_to_vector(name, "/");

    return kget_file(dir_lookup(v, 0, start->inode));
}


static inode_t dir_lookup(vector* v, size_t level, inode_t start) {
    assert(start);
    kfile* f = kget_file(start);
    dir_entry *de = GET_DIR_DATA(f); 
    dir_entry c;


    printk("start: %p (%u)", f, f->inode);
    for (int i = 0; i < FILES_PER_DIR; i++) {
        c = de[i];
        if (c.inode) {
            if (0 == strcmp(c.name, v->data[level])) {
                if (v->size == level + 1) {
                    PUT_DIR_DATA(f, false);
                    kput_file(f);
                    return c.inode;
                } else {
                    if (c.type != KFS_DIR) {
                        PUT_DIR_DATA(f, false);
                        kput_file(f);
                        return 0;
                    }
                    PUT_DIR_DATA(f, false);
                    kput_file(f);
                    return dir_lookup(v, level + 1, c.inode);
                }
            }
        }
    }
    return 0;
}

