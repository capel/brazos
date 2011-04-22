#include "stdlib.h"
#include "kfs.h"
#include "mem.h"
#include "kvector.h"

#define FILES_PER_DIR 8
#define FILENAME_LEN 32

typedef struct _dir_entry {
    char name[FILENAME_LEN];
    kfile* file;
} dir_entry;

static kfile* root;



kfile* kf_dir_lookup(vector* v, size_t level, kfile* start);

void ksetup_fs() {
    root = kf_create(KFS_DIR); // root
    root->dir_name = "/";
}

void kf_makedir(kfile* f) {
    f->data = kcalloc(FILES_PER_DIR, sizeof(dir_entry));
    f->size = FILES_PER_DIR * sizeof(dir_entry);
    f->dir_name = 0;
}

kfile* kf_create(int type) {
    kfile* f = kmalloc(sizeof(kfile));
    f->size = 0;
    f->data = 0;
    f->type = type;
    f->dir_name = "ERROR: dir_parent on FILE";
    f->ref_count = 0; // bad to init like this, but our rf is 0

    if (type == KFS_DIR) {
        kf_makedir(f);
    }
    return f;
}

bool kf_add_to_dir(const char * name, kfile* f, kfile *dir) {
    dir_entry* de = (dir_entry*)dir->data;
    
    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].file == 0) {
            strlcpy(de[i].name, name, FILENAME_LEN);
            de[i].file = f;
            f->ref_count++;
            if (f->type == KFS_DIR && f->dir_name == 0) {
                f->dir_name = de[i].name;
            }

            return true;
        }
    }
    printk("No space left in dir %p", f);
    return false;
}

bool kf_rm_from_dir(kfile* f, kfile *dir) {
    dir_entry* de = (dir_entry*)dir->data;
    
    for(size_t i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].file == f) {
            de[i].file = 0;
            f->ref_count--;
            return true;
        }
    }
    return false;
}

kfile* kf_get_root() {
    return root;
}

void kf_close(kfile* f) {
    if (f->ref_count == 0) {
        kfree(f->data);
        kfree(f);
    }
}

bool kf_get_dir_entries(kfile *dir, void* space, size_t size) {
    if (size < FILES_PER_DIR * sizeof(dir_entry)) 
        return false;

    if (dir->type != KFS_DIR)
        return false;

    assert(size >= dir->size);
    memcpy(space, dir->data, dir->size);
    return true;
}

kfile* kf_lookup(const char* name, kfile *start) {
    assert(start);
    assert(start->type == KFS_DIR);

    bool abs = false;

    if (0 == strcmp(name, "/")) {
        return root;
    }

    if (name[0] == '/') {
        // root dir
        abs = true;
        name++;
    }

    vector *v = ksplit_to_vector(name, "/");

    if (abs) {
        return kf_dir_lookup(v, 0, root);
    } else {
        return kf_dir_lookup(v, 0, start);
    }
}


kfile* kf_dir_lookup(vector* v, size_t level, kfile* start) {
    dir_entry *de = (dir_entry*)start->data;

    for (int i = 0; i < FILES_PER_DIR; i++) {
        if (de[i].file) {
            if (0 == strcmp(de[i].name, v->data[level])) {
                if (v->size == level + 1) {
                    return de[i].file;
                } else {
                    if (de[i].file->type != KFS_DIR) {
                        return 0;
                    }
                    return kf_dir_lookup(v, level + 1, de[i].file);
                }
            }
        }
    }
    return 0;
}





size_t kf_write(kfile* f, char* buf, size_t len, size_t pos) {
    assert(f);

    if (f->data == 0) {
        f->data = kmalloc(pos+len);
        f->size = pos+len;
    } else if ((pos + len) > f->size) {
        f->data = krealloc(f->data, pos + len);
        memset(f->data+f->size, 0, pos + len - f->size);
        f->size = pos+len;
    }

    memcpy(f->data + pos, buf, len);

    return len;
}

size_t kf_read(kfile* f, char* buf, size_t len, size_t pos) {
    if (!f)
        return 0;

    if (pos > f->size)
        return 0;

    size_t diff = f->size - pos;

    memcpy(buf, f->data+pos, MIN(diff, len));

    return MIN(diff, len);
}

int kflush_dirty_file(kfile *f) {
    kinode * i = f->inode_ptr;
    i->size = f->size;
    i->type = f->type;
    i->ref_count = f->ref_count;

    size_t num_blocks = f->size / BLOCK_SIZE; 

    char block[BLOCK_SIZE];

    for(size_t block = 0; block < num_blocks; i++) {
        size_t diff = f->size - block * BLOCK_SIZE;
        if (diff < BLOCK_SIZE) {
            memset(block, 0, BLOCK_SIZE);
            memcpy(block, f->data + block * BLOCK_SIZE, diff);
            kwrite_block(i->dblocks[block], block);
        } else {
            kwrite_block(i->dblocks[block], f->data + block * BLOCK_SIZE);
        }
    }

    write_inode(i);
}
