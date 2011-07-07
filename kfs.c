#include "stdlib.h"
#include "kfs.h"
#include "mem.h"
#include "kvector.h"
#include "bcache.h"
#include "hashmap.h"
#include "malloc.h"
#include "procfs.h"

#define NUM_FILES_PW2 4
#define ROOT_INODE 1

#define TYPE_ROOT (1 << 8)

static hashmap* file_map;
static kfile* _root;

void kfs_register_file(kfile* f) {
    hm_insert(file_map, f->inode, f);
}

void vfs_bad_func() {
    panic("Bad vfs func called.");
}


void ksetup_fs() {
    file_map = make_hashmap(NUM_FILES_PW2, &kernel_alloc_funcs);
    _root = kget_file(ROOT_INODE);
    _root->ref_count = 1337;
    printk("_root name %s", _root->dir_name);
    // TODO not hardcode stuff here.
    kf_setup_new_dir(_root);
    _root->add_file(_root, "..", _root);
    _root->dir_name = "/";

    kfile* proc = setup_procfs();
    _root->add_file(_root, "proc", proc);
    proc->add_file(proc, "..", _root);
}

kfile* root() {
    return _root;
}

void kf_copy_from_inode(kfile * f, const kinode* inode) {
    f->inode = inode->inode;
    f->ref_count = 0;
    f->size = inode->size;
    f->type = inode->flags;
    f->link_count = inode->link_count;
    for(size_t i = 0; i < NUM_DBLOCKS; i++) {
        f->dblocks[i] = inode->dblocks[i];
    }

    f->private_data = NULL;
}


kfile* kget_file(inode_t inum) {
    if (inum == 0)
        return 0;
    
    kfile* f = hm_lookup(file_map, inum);
    if (!f) {
        printk("file %d not found, loading", inum);
        f = kmalloc(sizeof(kfile));
        kinode * inode = kget_inode(inum);
        assert(inode);

        kf_copy_from_inode(f, inode);
        if (f->type == KFS_DIR) {
            kf_setup_dir(f);
        } else {
            kf_setup_normal_file(f);
        }
        kput_inode(inode, false);
        kfs_register_file(f);
        assert(hm_lookup(file_map, inum));
    }

    f->ref_count++;
    return f;
}

kfile* kf_create(int type) {
    kfile* f = kmalloc(sizeof(kfile));
    f->inode = kalloc_inode();
    //printk("inode %d", f->inode);
    
    // setup empty inode
    kinode* inode = kget_inode(f->inode);
    inode->inode = f->inode;
    inode->size = 0;
    inode->flags = type;
    inode->link_count = 0;
    memset((void*)inode->dblocks, 0, sizeof(disk_addr) * NUM_DBLOCKS);

    kf_copy_from_inode(f, inode);

    if (type == KFS_DIR) {
        kf_setup_new_dir(f);
    } else if (type == KFS_NORMAL_FILE) {
        kf_setup_new_normal_file(f);
    }
    
    kput_inode(inode, true);
    f->ref_count = 1;

    kfs_register_file(f);
    assert(hm_lookup(file_map, f->inode) == f);

    return f;
}


void _basic_delete_self(kfile* f) {
    // TODO
}

void kput_file(kfile* f) {
 //   kflush_file(f);

    f->ref_count--;
    if (f->ref_count == 0) {
        printk("Ref count is zero");
        if (f->link_count == 0) {
            printk("About to delete self");
            f->delete_self(f);
        }
        kfree(f);
    }
}

static inline disk_addr find_inode_block(size_t inode) {
    return INODE_START + inode / INODES_PER_BLOCK;
}

static inline size_t find_inode_offset(size_t inode) {
    return inode % INODES_PER_BLOCK;
}

kinode* kget_inode(size_t inode) {
    disk_addr block = find_inode_block(inode);
    size_t offset = find_inode_offset(inode);

  //  printk("block %d offset %d sizeof %d", block, offset, sizeof(kinode));

    kinode *inodes = kget_block(block);
    kinode* i = kmalloc(sizeof(*i));
    memcpy((void*)i, (void*)&inodes[offset], sizeof(*i));
  //  printk("inode %d size %d flags %d lc %d dblocks[0] %d", i->inode,
  //      i->size, i->flags, i->link_count, i->dblocks[0]);
    kput_block(block, false);
    return i;
}


void kput_inode(const kinode* i, bool dirty) {
    if (!dirty) {
        printk("Not diry, just freeing %p", i);
        //kfree((void*)i);
        return;
    }

    disk_addr block = find_inode_block(i->inode);
    size_t offset = find_inode_offset(i->inode);

   // printk("Inode info: block %d offset %d", block, offset);

    kinode * inodes = kget_block(block);
    memcpy((void*) &inodes[offset], (void*)i, sizeof(*i));
    kput_block(block, true);
    kfree((void*)i);
}



static inline size_t write_block(kfile *f, const char* buf, size_t len, size_t pos) {
    disk_addr baddr = f->dblocks[pos / BLOCK_SIZE];
    void * block;
    if (baddr == 0) {
        baddr = f->dblocks[pos / BLOCK_SIZE] = kalloc_block();
        assert(baddr);
        block = kget_block(baddr);
        memset(block, 0, BLOCK_SIZE);
    } else {
        block = kget_block(baddr);
    }
    
    size_t to_write = MIN(len, BLOCK_SIZE - (pos %  4096));
    printk("to_write %d, len %d, other %d", to_write, len, BLOCK_SIZE - (pos%4096));

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

size_t _file_write(kfile* f, const char* buf, size_t len, size_t pos) {
    assert(buf);
    
    if (pos + len > f->size) {
        f->size = pos + len;
    }
   
    size_t old_len = len;

    while(len > 0) {
        printk("len %d", len);
        size_t written = write_block(f, buf, len, pos); 
        pos += written;
        buf += written;
        len -= written;
    }

    return old_len;
}

size_t _file_read(kfile* f, char* buf, size_t len, size_t pos) {
    assert(buf);
   
    // read only as much as exists.
    if (pos + len > f->size) {
        len = f->size - pos;
    }
   
    size_t old_len = len;

    while(len) {
        size_t written = read_block(f, buf, len, pos); 
        pos += written;
        buf += written;
        len -= written;
    }

    return old_len;
}

int _basic_flush(kfile *f) {
    printk("In basic flush");
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

    printk("Done with flush");
    return 0;
}

void kf_setup_new_normal_file(kfile * f) {
    kf_setup_normal_file(f);
}
void kf_setup_normal_file(kfile * f) {
    f->dir_name = "ERROR: dir_parent on FILE";
    f->private_data = NULL;
    
    f->flush = _basic_flush;
    f->delete_self = _basic_delete_self;

    // file funcs
    f->write = _file_write;
    f->read = _file_read;
    
    // directory funcs -- shouldn't be called
    f->get_entries = (void*) vfs_bad_func;
    f->add_file = (void*) vfs_bad_func;
    f->rm_file = (void*) vfs_bad_func;
    f->lookup_file = (void*) vfs_bad_func;
}

