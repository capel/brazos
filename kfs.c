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



static ent* _root;
ent* root() {
    return _root;
}

static hashmap* file_map;
void kfs_register_file(kfile* f) {
    hm_insert(file_map, f->inode, f);
}

void ksetup_fs() {
    file_map = make_hashmap(NUM_FILES_PW2, &kernel_alloc_funcs);

    _root = kcreate_dir();
    kget(_root); // it contains itself!

    ent* proc = kcreate_proc_manager();
    vector* v = ksplit_to_vector("proc", "/");
    LINK(_root, KERNEL, proc, v, 0);
}

const char* kitoa(int i) {
  char* s = kmalloc(12);
  snprintf(s, 12, "%d", i);
  return s;
}

