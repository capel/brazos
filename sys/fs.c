#include "ko.h"

ko* mk_dir(void);

static ko* fs_mk_dir(ko* o) { return mk_dir(); }

ko* mk_fs(void) {
  ko* fs = mk_dir();
  ko* mkdir = bind0((bound_func)fs_mk_dir, NULL);
  LINK(fs, mkdir, "mkdir");
  kput(mkdir);
  return fs;
}
