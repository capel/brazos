#ifndef PROCFS_H
#define PROCFS_H

#ifdef USER
#error __FILE__ " included from a user context."
#endif

#include "types.h"
#include "kfs.h"
#include "sched.h"

kfile* setup_procfs(void);
kfile* setup_procfile(proc* p);

#endif
