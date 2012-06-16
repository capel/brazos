
#include <file.h>
#include <mach/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <extras.h>
#include <dir.h>

#include "../fs.h"
#include "path_util.h"
#include "common.h"

struct dir_desc {
  Link* link;
  int pos;
};

fs_state* st();

int _opendir(const char* orig_path, int flags) {
  // the new value must be freed
  // we can just ignore the old value
  char* path = path_normalize(get_cwd(st()), orig_path);
  assert(path);

  Node * n = walk(path);
  if (!n) {
    if (!(flags & _O_CREAT)) {
      free((char*) path);
      return E_NOTFOUND;
    } else {
      char * parent = path_parent(path);
      Node * parent_dir = walk(parent);
      free((char*)parent);

      if (!parent_dir) return E_NOTFOUND;
      if (!is_dir(parent_dir)) return E_INVAL;

      n = NODE(dir_ctor(0));

      char * name = path_name(path);
      dir_add(get_dir(parent_dir), name, n);
      free(name);
    }
  }

  if (!is_dir(n)) {
    free((char*) path);
    return E_INVAL;
  }

  dir_desc * dd = malloc(sizeof(dir_desc));
  dd->link = link_ctor(strclone(path));
  dd->pos = 0;

  free((char*) path);
  return state_ctor_dd(st(), dd);
}

int _nextfile(int _dd, struct _stat_entry* out) {
  dir_desc* dd = state_dd(st(), _dd);
  if (!dd) return E_BADFD;

  if (!out) return E_INVAL;

  Node * n = walk(dd->link);
  if (!n) return E_CANT;
  if (!is_dir(n)) return E_INVAL;

  Directory* d = get_dir(n);

  int ret = dir_stat(d, dd->pos, out);
  if (ret != 0) return ret;
  dd->pos++;

  return 0;
}

int _closedir(int _dd) {
  dir_desc* dd = state_dd(st(), _dd);
  if (!dd) return E_BADFD;

  Sync(dd->link);
  DTOR(dd->link);

  free(dd);
  state_dtor_dd(st(), _dd);
  return 0;
}
