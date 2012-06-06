
#include <file.h>
#include <mach/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <extras.h>

#include "path_util.h"
#include "fs.h"

#define MAX_FDS 64

typedef struct dir_desc {
  Link* link;
  int pos;
} dir_desc;

static dir_desc* fd_table[MAX_FDS];

static int allocate_fd() {
  for (int i = 1; i < MAX_FDS; i++) {
    if (fd_table[i] == 0) {
      return i;
    }
  }
  return E_FULL;
}

int _opendir(const char* orig_path, int flags) {
  int fd = allocate_fd();
  if (fd == E_FULL) return E_FULL;

  // the new value must be freed
  // we can just ignore the old value
  const char* path = path_normalize(get_cwd(), orig_path);
  assert(path);

  Node * n = walk(path);
  if (!n) {
    if (!(flags & _O_CREAT)) {
      free((char*) path);
      return E_NOTFOUND;
    } else {
      Directory* d = dir_ctor(0);
      n = NODE(d);
      const char * parent = path_parent(path);
      const char * name = path_name(path);

      Node * parent_dir = walk(parent);
      assert(parent_dir);

      free((char*)parent);
      Directory* dir = get_dir(parent_dir);
      dir_add(dir, name, n);
    }
  }

  if (!is_dir(n)) {
    free((char*) path);
    return E_INVAL;
  }

  dir_desc * f = malloc(sizeof(dir_desc));
  f->link = link_ctor(strclone(path));
  f->pos = 0;

  fd_table[fd] = f;

  free((char*) path);
  return fd;
}

int _nextfile(int fd, char* buf, size_t len) {
  if (fd > MAX_FDS) return E_BADFD;

  dir_desc* f = fd_table[fd];
  if (!f) return E_BADFD;

  Node * n = walk(f->link);
  if (!n) return E_CANT;
  if (!is_dir(n)) return E_INVAL;

  Directory* d = get_dir(n);

  const char* s = dir_entry(d, f->pos);
  if (!s) return E_NOTFOUND;

  strncpy(buf, s, len);
  f->pos++;
  return 0;
}

int _closedir(int fd) {
  if (fd > MAX_FDS) return E_BADFD;

  dir_desc* f = fd_table[fd];
  if (!f) return E_BADFD;

  Sync(f->link);
  DTOR(f->link);

  free(f);
  fd_table[fd] = 0;
  return 0;
}
