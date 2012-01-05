#include "ko.h"

dir* mk_queue(void);

static ko* fs_mk_dir() { return KO(mk_dir()); }
static ko* fs_mk_queue() { return KO(mk_queue()); }

dir* mk_fs(void) {
  dir* fs = mk_dir();
  SAFE_ADD(fs, mk_ctor(fs_mk_dir, KO_DIR), "mkdir");
  SAFE_ADD(fs, mk_ctor(fs_mk_queue, KO_DIR), "queue");
  printk("Fs %k", fs);
  return fs;
}
