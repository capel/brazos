#ifndef SYS_DIR_H
#define SYS_DIR_H

#include "khashmap.h"
#include "kihashmap.h"
#include "ko.h"

typedef struct {
  ko o;
  khashmap* h;
} dir;

typedef struct {
  ko o;
  kihashmap* h;
} numdir;

#endif
