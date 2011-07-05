#include "stdlib.h"
#include "kfs.h"
#include "mem.h"
#include "kvector.h"
#include "bcache.h"

int _basic_flush(kfile* f);
void _basic_delete_self(kfile * f);


void print_dir(kfile* dir) {
    assert(dir);
    assert(dir->type = KFS_DIR);
    printk("About to get entries");
   // printk("f: %p :: get_entries %p", dir->get_entries(dir), _dir_get_dir_entries)
    dir_entry* de = dir->get_entries(dir);
    printk("DIR: %s (%d:%p)", dir->dir_name, dir->inode, dir);
    printk("dir block %d", dir->dblocks[0]);
    if (de) {
        printk("dir block addr %p", de);
        for (size_t i = 0; i < FILES_PER_DIR; i++) {
            if (de[i].inode) {
                printk("* %s (%d) : %d", de[i].name, de[i].inode, de[i].type);
            }
        }
        dir->put_entries(dir, false);
    } else {
        printk("UNALLOCED dir_block. WTF?!");
    }

}

static inode_t dir_lookup(vector* v, size_t level, inode_t i);

bool _dir_add_file(kfile* dir, const char * name, kfile* f) {
    dir_entry* de = dir->get_entries(dir); 
    assert(de);

    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].inode == 0) {
            strlcpy(de[i].name, name, FILENAME_LEN);
            de[i].inode = f->inode;
            de[i].type = f->type;
            printk("Type %d", de[i].type);
            f->link_count++;
            printk("Adding %s (%d) to %s (%d)", name, 
                f->inode, dir->dir_name, dir->inode);
            if (f->type == KFS_DIR && f->dir_name == 0) {
                f->dir_name = de[i].name;
            }

            dir->flush(dir);
            dir->put_entries(dir, true);
            return true;
        }
    }
    printk("No space left in dir %s (%d)", f->dir_name, f->inode);
    print_dir(f);
    dir->put_entries(dir, false);
    return false;
}

bool _dir_rm_file(kfile* dir, kfile* f) {
    dir_entry* de = dir->get_entries(dir); 
    
    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].inode == f->inode) {
            de[i].inode = 0;
            dir->put_entries(dir, true);
            f->link_count--;
            return true;
        }
    }
    dir->put_entries(dir, false);
    return false;
}

dir_entry* _dir_get_dir_entries(kfile * dir) {
    assert(dir->dblocks[0] != 0);
    return kget_block(dir->dblocks[0]);
}

void _dir_put_dir_entries(kfile* dir, bool dirty) {
    kput_block(dir->dblocks[0], dirty);
}

bool kf_copy_dir_entries(kfile *dir, void* space, size_t size) {
    assert(size >= FILES_PER_DIR * sizeof(dir_entry)); 
    assert(dir->type == KFS_DIR);
    printk("size: %d dir->size %d", size, dir->size);
    assert(size >= dir->size);

    dir_entry* de = dir->get_entries(dir); 
    printk("first: ", de[0].name);

    memcpy(space, (char*) de, dir->size);
    dir->put_entries(dir, false);
    return true;
}

kfile* kf_lookup(const char* name, kfile *start) {
    assert(start);
    assert(start->type == KFS_DIR);
    assert(start->inode);
    
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
    dir_entry *de = f->get_entries(f); 
    dir_entry c;


    printk("start: %p (%u)", f, f->inode);
    for (int i = 0; i < FILES_PER_DIR; i++) {
        c = de[i];
        if (c.inode) {
            if (0 == strcmp(c.name, v->data[level])) {
                if (v->size == level + 1) {
                    f->put_entries(f, false);
                    kput_file(f);
                    return c.inode;
                } else {
                    if (c.type != KFS_DIR) {
                        f->put_entries(f, false);
                        kput_file(f);
                        return 0;
                    }
                    f->put_entries(f, false);
                    kput_file(f);
                    return dir_lookup(v, level + 1, c.inode);
                }
            }
        }
    }
    return 0;
}

void kf_setup_new_dir(kfile* f) {
    kf_setup_dir(f);
    printk("Setup dir inode: %d", f->inode);
    f->size = FILES_PER_DIR * sizeof(dir_entry);
    f->dir_name = NULL;
    f->dblocks[0] = kalloc_block();
    void * b = kget_block(f->dblocks[0]);
    memset(b, 0, PAGE_SIZE);
    kput_block(f->dblocks[0], true);
    printk("Out of setup");
}
    
void kf_setup_dir(kfile * f) {
    f->private_data = NULL;

    f->delete_self = _basic_delete_self;
    f->flush= _basic_flush;
    // directory funcs
    f->get_entries = _dir_get_dir_entries;
    f->put_entries = _dir_put_dir_entries;
    f->add_file = _dir_add_file;
    f->rm_file = _dir_rm_file;
    
    // file funcs -- shouldn't be called.
    f->write = (void*)vfs_bad_func;
    f->read = (void*)vfs_bad_func;
}

