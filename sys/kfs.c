#include "include/common.h"

static ent* _root;
ent* root() {
    return _root;
}

void kcreate_root() {
    _root = kcreate_dir();
    kget(_root); // it contains itself!
}


