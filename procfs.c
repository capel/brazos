#include "sched.h"
#include "kfs.h"
#include "mem.h"
#include "stdlib.h"
#include "vfs.h"

void _basic_delete_self(kfile * f);
kfile* setup_messagefile(const char* msg);

static int noop() { return 0; }

static kfile * procfs_dir;

LOOKUP_FUNC(get_current_dir) { return kget_file_raw(f); }
LOOKUP_FUNC(get_procfs_dir) { return procfs_dir; }
LOOKUP_FUNC(get_parent) {return NULL; }
LOOKUP_FUNC(get_pid) { 
    return setup_messagefile(kitoa(((proc*)(f->private_data))->pid));
}
LOOKUP_FUNC(not_found) { return NULL; }
LOOKUP_FUNC(get_root) { return root(); }
LOOKUP_FUNC(get_current_proc_dir) { return kget_procfile(cp()); }

LOOKUP_FUNC(get_proc_dir) {
    proc* p = proc_by_pid(atoi(name));
    return p ? kget_procfile(p) : NULL;
}

static dir_entry proc_dir_entries[] = {
    {".", VIRTUAL_INODE, KFS_DIR},
    {"..", VIRTUAL_INODE, KFS_DIR},
    {"parent", VIRTUAL_INODE, KFS_NORMAL_FILE},
    {"pid", VIRTUAL_INODE, KFS_NORMAL_FILE},
    EMPTY_DE,
    EMPTY_DE,
    EMPTY_DE,
    EMPTY_DE
};

static lookup_entry proc_lookup_entries[] = {
    {".", get_current_dir},
    {"..", get_procfs_dir},
    {"parent", get_parent},
    {"pid", get_pid},
    {DEFAULT_LOOKUP_FUNC, not_found},
};

static lookup_entry procfs_lookup_entries[] = {
    {".", get_current_dir},
    {"..", get_root},
    {"me", get_current_proc_dir},
    {DEFAULT_LOOKUP_FUNC, get_proc_dir}
};

static bool rm(kfile* f, kfile* rm) {
    proc * p = rm->private_data;
    if (!p) {
        return false;
    }
    kfree_proc(p);
    return true;;
}

static void procfs_put_entries(kfile* f, bool dirty) {
    assert(!dirty);
    kfree(f->private_data);
}

LOOKUP_FUNC_FACTORY(procfs, procfs_lookup_entries,
        SIZEOF_ARRAY(procfs_lookup_entries));
LOOKUP_FUNC_FACTORY(proc, proc_lookup_entries, 
        SIZEOF_ARRAY(proc_lookup_entries));

static dir_entry* procfs_get_entries(kfile* f) {
    f->private_data = kmalloc(DIR_ENTRIES_SIZE);
    dir_entry* entries = f->private_data;

    // Make . and ..
    strlcpy(entries[0].name, ".", FILENAME_LEN);
    entries[0].inode = VIRTUAL_INODE;
    entries[0].type = KFS_DIR;

    strlcpy(entries[1].name, "..", FILENAME_LEN);
    entries[1].inode = VIRTUAL_INODE;
    entries[1].type = KFS_DIR;

    size_t dir_pos = 2;
    size_t proc_pos = 0;
    for(; dir_pos < FILES_PER_DIR && proc_pos < PROC_TABLE_SIZE; proc_pos++) {
        proc * p = proc_by_pos(proc_pos);
        if (!p) {
            continue;
        }
        entries[dir_pos].inode = VIRTUAL_INODE;
        entries[dir_pos].type = KFS_DIR;
        snprintf(entries[dir_pos].name, FILENAME_LEN, "%d", p->pid);
        dir_pos++;
    }

    return entries;
}

static dir_entry* proc_get_entries(kfile* f) {
    return proc_dir_entries;    
}


kfile* setup_procfs() { 
    kfile * f = kmalloc(sizeof(kfile));
    f->inode = kalloc_inode();
    f->size = FILES_PER_DIR * sizeof(dir_entry);
    f->ref_count = 1337;
    f->dir_name = "proc";
    f->type = KFS_DIR;
    f->private_data = NULL;

    f->flush = (void*)noop;
    f->delete_self = (void*)noop;
    f->get_entries = procfs_get_entries;
    f->put_entries = procfs_put_entries;
    f->lookup_file = LOOKUP_FUNC_FACTORY_NAME(procfs);
    f->add_file = 0;
    f->rm_file = rm;

    f->write = 0;
    f->read = 0;
    
    kfs_register_file(f);
    procfs_dir = f;
    return f;
}

kfile * setup_procfile(proc* p) {
    printk("New procfile for pid %d at %p", p->pid, p);
    kfile * f = kmalloc(sizeof(kfile));
    f->inode = VIRTUAL_INODE;
    f->ref_count = 1;
    f->dir_name = kitoa(p->pid); 
    printk("Dir name: %s", f->dir_name);
    f->type = KFS_DIR;
    f->private_data = p;

    f->flush = (void*)noop;
    f->delete_self = _basic_delete_self;
    f->get_entries = proc_get_entries;
    f->put_entries = (void*)noop;
    f->lookup_file = LOOKUP_FUNC_FACTORY_NAME(proc);

    f->add_file = 0;
    f->rm_file = 0;

    f->write = 0;
    f->read = 0;
    
    return f;
}

size_t message_read(kfile* f, char* buf, size_t len, size_t pos) {
    size_t to_read = MIN(len, strlen(f->private_data) + 1 - pos);
    strlcpy(buf, f->private_data+pos, to_read);
    return to_read;
}

void delete_self_message(kfile* f) {
    kfree(f->private_data);
    kfree(f);
}

kfile* setup_messagefile(const char* message) {
    printk("New message file for %s", message);
    kfile * f = kmalloc(sizeof(kfile));
    f->inode = VIRTUAL_INODE;
    f->ref_count = 1;
    f->dir_name = 0;
    f->type = KFS_NORMAL_FILE;
    f->private_data = (void*) message;

    f->flush = (void*)noop;
    f->delete_self = delete_self_message;
    f->get_entries = 0;
    f->put_entries = 0;
    f->lookup_file = 0;

    f->add_file = 0;
    f->rm_file = 0;

    f->write = (void*)noop;
    f->read = message_read;
    
    return f;
}
