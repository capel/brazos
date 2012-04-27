#include "include/kfs.h"
#include "include/mem.h"
#include "../common/stdlib.h"
#include "include/kio.h"

static ent* _root;
ent* root() {
    return _root;
}

void ksetup_fs() {
    _root = kcreate_dir();
    kget(_root); // it contains itself!
}


