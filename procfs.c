#include "sched.h"
#include "kfs.h"
#include "mem.h"
#include "stdlib.h"

bool _procfs_add(kfile* f, const char * name, struct _kfile* add) {
    return false;
}

bool _procfs_rm(kfile* f, kfile* rm) {
    return false;
}

int _procfs_flush(kfile* f) {
    return 0;
}

void _procfs_delete_self(kfile * f) {
    return;
}

void _procfs_put_entries(kfile* f, bool dirty) {
    assert(!dirty);
    kfree(f->private_data);
}

dir_entry* _procfs_get_entries(kfile* f) {
    f->private_data = kmalloc(sizeof(dir_entry) * FILES_PER_DIR);
    dir_entry* entries = f->private_data;
    size_t dir_pos = 0;
    size_t proc_pos = 0;
    for(; dir_pos < FILES_PER_DIR && proc_pos < PROC_TABLE_SIZE; proc_pos++) {
        proc * p = proc_by_pos(proc_pos);
        if (!p) {
            continue;
        }
        printk("adding %p [%d]", p, proc_pos);
        entries[dir_pos].inode = p->inode;
        entries[dir_pos].type = KFS_NORMAL_FILE;
        snprintf(entries[dir_pos].name, FILENAME_LEN, "proc_%d", p->pid);
        printk("name: %s", entries[dir_pos].name);
    }

    return entries;
}

kfile* setup_procfs() { 
    kfile * f = kmalloc(sizeof(kfile));
    f->inode = kalloc_inode();
    f->size = FILES_PER_DIR * sizeof(dir_entry);
    f->ref_count = 1337;
    f->dir_name = "proc";
    f->type = KFS_DIR;
    f->private_data = NULL;

    f->flush = _procfs_flush;
    f->delete_self = _procfs_delete_self;
    f->get_entries = _procfs_get_entries;
    f->put_entries = _procfs_put_entries;
    f->add_file = _procfs_add;
    f->rm_file = _procfs_rm;

    f->write = (void*) vfs_bad_func;
    f->read = (void*) vfs_bad_func;
    
    kfs_register_file(f);
    return f;
}
