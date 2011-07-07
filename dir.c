#include "stdlib.h"
#include "kfs.h"
#include "mem.h"
#include "kvector.h"
#include "bcache.h"

int _basic_flush(kfile* f);
void _basic_delete_self(kfile * f);

bool _dir_add_file(kfile* dir, const char * name, kfile* f) {
    dir_entry* de = dir->get_entries(dir); 
    assert(de);

    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].inode == 0) {
            strlcpy(de[i].name, name, FILENAME_LEN);
            de[i].inode = f->inode;
            de[i].type = f->type;
            // prevent infinite loops
            if (dir != f) {
                f->link_count++;
            }
            if (f->type == KFS_DIR && f->dir_name == 0) {
                f->dir_name = kstrclone(de[i].name);
            }

            //dir->flush(dir);
            dir->put_entries(dir, true);
            return true;
        }
    }
    printk("No space left in dir %s (%d)", f->dir_name, f->inode);
    dir->put_entries(dir, false);
    return false;
}

bool _dir_rm_file(kfile* dir, kfile* f) {
    printk("Rm %d from %s", f->inode, dir->dir_name);
    dir_entry* de = dir->get_entries(dir); 
    
    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].inode == f->inode) {
            de[i].inode = 0;
            strlcpy(de[i].name, "", FILENAME_LEN);
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
    assert(size >= DIR_ENTRIES_SIZE);
    assert(dir->type == KFS_DIR);

    dir_entry* de = dir->get_entries(dir); 

    memcpy(space, (char*) de, DIR_ENTRIES_SIZE);
    dir->put_entries(dir, false);
    return true;
}

kfile* kf_lookup(const char* name, kfile *start) {
    assert(start);
    assert(start->type == KFS_DIR);
    
    if (0 == strcmp(name, "/")) {
        return root();
    }

    if (name[0] == '/') {
        // root dir
        start = root();
        name++;
    }

    vector *v = ksplit_to_vector(name, "/");

    return start->lookup_file(start, v, 0);
}


kfile * _dir_lookup_file(kfile* dir, vector* v, size_t level) {
    dir_entry *de = dir->get_entries(dir); 
    dir_entry c;


    for (int i = 0; i < FILES_PER_DIR; i++) {
        c = de[i];
        if (c.inode == 0) {
            continue;
        }
        if (0 == strcmp(c.name, v->data[level])) {
            // we are at the end of the path
            if (v->size == level + 1) {
                dir->put_entries(dir, false);
                kput_file(dir);
                return kget_file(c.inode);
            } else {
                if (c.type != KFS_DIR) {
                    dir->put_entries(dir, false);
                    kput_file(dir);
                    return 0;
                }
                dir->put_entries(dir, false);
                kfile * next = kget_file(c.inode);
                kput_file(dir);
                return next->lookup_file(next, v, level + 1);
            }
        }
    }
    dir->put_entries(dir, false);
    kput_file(dir);
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
    f->add_file(f, ".", f);
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
    f->lookup_file = _dir_lookup_file;
    
    // file funcs -- shouldn't be called.
    f->write = (void*)vfs_bad_func;
    f->read = (void*)vfs_bad_func;
}

