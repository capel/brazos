#include "stdlib.h"
#include "kfs.h"
#include "mem.h"
#include "kvector.h"
#include "bcache.h"
#include "hashmap.h"
#include "malloc.h"

#define NUM_FILES_PW2 4

kfile* root;

static hashmap* file_map;

void ksetup_fs() {
    file_map = make_hashmap(NUM_FILES_PW2, &kernel_alloc_funcs);
    root = kget_file(ROOT_INODE); // root
    root->dir_name = "/";
}

kfile* kget_file(inode_t inum) {
    assert(inum != 0);
    kfile* f = hm_lookup(file_map, inum);
    if (!f) {
        f = kmalloc(sizeof(kfile));
        f->inode = inum;
        f->ref_count = 0;
        kinode * inode = kget_inode(inum);
        assert(inode);

        f->size = inode->size;
        f->type = inode->flags;
        f->link_count = inode->link_count;
        for(size_t i = 0; i < NUM_DBLOCKS; i++) {
            f->dblocks[i] = inode->dblocks[i];
        }
        kput_inode(inode, false);
    }

    f->ref_count++;
    return f;
}

kfile* kf_create(int type) {
    kfile* f = kmalloc(sizeof(kfile));
    f->inode = kdisk_get_free_inode();
    f->size = 0;
    f->type = type;
    f->dir_name = "ERROR: dir_parent on FILE";
    f->ref_count = 1;

    if (type == KFS_DIR) {
        kf_mkdir(f);
    }
    return f;
}

kfile* kf_get_root() {
    return root;
}

void kf_delete(kfile* f) {
    // TODO
    f = 0;
}

void kput_file(kfile* f) {
    kflush_file(f);

    f->ref_count--;
    if (f->ref_count == 0) {
        if (f->link_count == 0) {
            kf_delete(f);
        }
        kfree(f);
    }
}

static inline disk_addr find_inode_block(size_t inode) {
    return INODE_BLOCK + inode / INODES_PER_BLOCK;
}

static inline size_t find_inode_offset(size_t inode) {
    return inode % INODES_PER_BLOCK;
}

kinode* kget_inode(size_t inode) {
    disk_addr block = find_inode_block(inode);
    size_t offset = find_inode_offset(inode);

    kinode *inodes = kget_block(block);
    kinode* i = malloc(sizeof(*i));
    memcpy((void*)i, (void*)&inodes[offset], sizeof(*i));
    kput_block(block, false);
    return i;
}


void kput_inode(const kinode* i, bool dirty) {
    if (!dirty) {
        kfree((void*)i);
        return;
    }

    disk_addr block = find_inode_block(i->inode);
    size_t offset = find_inode_offset(i->inode);

    kinode * inodes = kget_block(block);
    memcpy((void*) &inodes[offset], (void*)i, sizeof(*i));
    kput_block(block, true);
    kfree((void*)i);
}



static inline size_t write_block(kfile * f, char* buf, size_t len, size_t pos) {
    disk_addr baddr = f->dblocks[pos / BLOCK_SIZE];
    void * block;
    if (baddr == 0) {
        baddr = f->dblocks[pos / BLOCK_SIZE] = kfind_free_block();
        assert(baddr);
        block = kget_block(baddr);
        memset(block, 0, BLOCK_SIZE);
    } else {
        block = kget_block(baddr);
    }
    
    size_t to_write = MIN(len, BLOCK_SIZE - (pos %  4096));

    memcpy(block + (pos % 4096), buf, to_write);

    kput_block(baddr, false);
    return to_write;
}

static inline size_t read_block(kfile * f, char* buf, size_t len, size_t pos) {
    disk_addr baddr = f->dblocks[pos / BLOCK_SIZE];
    void * block;
    size_t to_read = MIN(len, BLOCK_SIZE - (pos %  4096));
    
    // if we haven't allocated a block, it's
    // a file hole, so we just return 0s.
    if (baddr == 0) {
        memset(buf, 0, to_read);
        return to_read;
    } else {
        block = kget_block(baddr);
    }
    
    size_t to_write = MIN(len, BLOCK_SIZE - (pos %  4096));

    memcpy(buf, block + (pos % 4096), to_write);

    kput_block(baddr, true);
    return to_read;
}

size_t kf_write(kfile* f, const char* buf, size_t len, size_t pos) {
    assert(f);
    assert(buf);
    
    if (pos + len > f->size) {
        f->size = pos + len;
    }
   
    size_t old_len = len;

    while(len) {
        size_t written = write_block(f, buf, pos, len); 
        pos += written;
        buf += written;
        len -= written;
    }

    return old_len;
}

size_t kf_read(kfile* f, char* buf, size_t len, size_t pos) {
    assert(f);
    assert(buf);
   
    // read only as much as exists.
    if (pos + len > f->size) {
        len = f->size - pos;
    }
   
    size_t old_len = len;

    while(len) {
        size_t written = read_block(f, buf, pos, len); 
        pos += written;
        buf += written;
        len -= written;
    }

    return old_len;
}

int kflush_file(kfile *f) {
    kinode* i = kget_inode(f->inode);
    i->size = f->size;
    i->flags = f->type;
    i->link_count = f->link_count;

    for(size_t j = 0; j < NUM_DBLOCKS; j++) {
        disk_addr bnum = f->dblocks[j];
        i->dblocks[j] = bnum;
        if (bnum)
            kflush_block(bnum);
    }

    kput_inode(i, true);
    return 0;
}
