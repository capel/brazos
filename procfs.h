#ifndef PROCFS_H
#define PROCFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif

#include "types.h"
#include "kfs.h"

kfile* setup_procfs(void);

#endif
