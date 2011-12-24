#include "ko.h"

dir* mk_queue(void);

static ko* fs_mk_dir(ko* o) { return KO(mk_dir()); }
static ko* fs_mk_queue(ko* o) { return KO(mk_queue()); }

dir* mk_fs(void) {
  dir* fs = mk_dir();
  SAFE_ADD(fs, BIND(fs_mk_dir, NULL), "mkdir^");
  SAFE_ADD(fs, BIND(fs_mk_queue, NULL), "queue^");
  printk("Fs %k", fs);
  return fs;
}
