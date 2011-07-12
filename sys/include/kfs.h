#ifndef KFS_H
#define KFS_H

#include "../../common/types.h"
#include "ent.h"

typedef ent kfile;
typedef ent kdir;

void kcreate_root(void);

kfile* kcreate_file(void);
kdir* kcreate_dir(void);

kfile* kget_by_path(const char* name, kdir* start);

kdir* root(void);

#endif
