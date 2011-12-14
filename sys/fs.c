#include "ko.h"

ko* mk_dir(void);
ko* mk_queue(void);

static ko* fs_mk_dir(ko* o) { return mk_dir(); }
static ko* fs_mk_queue(ko* o) { return mk_queue(); }

ko* mk_fs(void) {
  ko* fs = mk_dir();
  SAFE_ADD(fs, bind0((bound_func)fs_mk_dir, NULL), "mkdir^");
  SAFE_ADD(fs, bind0((bound_func)fs_mk_queue, NULL), "queue^");
  return fs;
}
