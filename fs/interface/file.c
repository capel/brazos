#include <file.h>
#include <mach/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <extras.h>

#include "../fs.h"

#include "path_util.h"
#include "common.h"

fs_state* _st;
fs_state* st() { return _st; }

void fs_init() {
  _st = ctor_state();
  int _stdin = _open("/dev/stdin", _O_RDONLY);
  assert(_stdin == 0);
  int _stdout = _open("/dev/stdout", _O_WRONLY);
  assert(_stdout == 1);
}

int valid(int off) {
  if (off < 0) return 0;
  if (off > PAGE_SIZE) return PAGE_SIZE - 1;
  return off;
}

struct file_desc {
  Link* link;
  size_t offset;
  int flags;
};

void file_shutdown() {
  for(size_t i = 0; i < MAX_FDS; i++) {
    file_desc* fd = state_fd(st(), i);
    if (!fd) continue;
    _sync(i);
    _close(i);
  }
  
  printk("shutdown");
}

int _chdir(const char* orig_path) {
  char* path = path_normalize(get_cwd(st()), orig_path);

  Node * dir = walk(path);
  if (!dir) return E_NOTFOUND;
  if (!is_dir(dir)) return E_INVAL;

  set_cwd(st(), path);
  free(path);
  return 0;
}

int _open(const char *orig_path, int flags) {
  // the new value must be freed
  // we can just ignore the old value
  const char* path = path_normalize(get_cwd(st()), orig_path);
  printk("norm %s", path);

  Node * n = walk(path);
  if (!n) {
    if (!(flags & _O_CREAT)) {
      free((char*) path);
      return E_NOTFOUND;
    } else {
      char * parent = path_parent(path);
      Node * parent_dir = walk(parent);
      free(parent);

      if (!parent_dir) return E_NOTFOUND;
      if (!is_dir(parent_dir)) return E_INVAL;

      n = NODE(file_ctor(0,0,0));
      assert(n);

      char * name = path_name(path);
      dir_add(get_dir(parent_dir), name, n);
      free(name);
    }
  }

  if (is_dir(n)) {
    free((char*) path);
    return E_INVAL;
  }

  file_desc * f = malloc(sizeof(file_desc));
  f->link = link_ctor(strclone(path));

  f->offset = 0;
  f->flags = flags & (_O_RDWR); // mask out O_CREAT

  free((char*) path);
  return state_ctor_fd(st(), f);
}

int _close(int fd) {
  file_desc* f = state_fd(st(), fd);
  if (!f) return E_BADFD;

  Sync(f->link);

  DTOR(f->link);

  free(f);
  state_dtor_fd(st(), fd);
  return 0;
}

int _sync(int fd) {
  file_desc* f = state_fd(st(), fd);
  if (!f) return E_BADFD;

  Sync(f->link);
  return 0;
}

int _seek(int fd, int offset, int whence) {
  file_desc* f = state_fd(st(), fd);
  if (!f) return E_BADFD;

  switch (whence) {
    case _SEEK_SET:
      f->offset = valid(offset);
      return f->offset;
    case _SEEK_CUR:
      f->offset = valid(f->offset + offset);
      return f->offset;
    case _SEEK_END:
      /*
      n = resolve(f->link);
      if (is_file(n)) {
        File * file = get_file(n);
        f->offset = valid(file->size + offset);
        file->size = f->offset;
      } else { 
        return E_INVAL;
      }
      return f->offset;
      */
    default:
      return E_INVAL;
  }
}

int _read(int fd, void *buf, size_t nbytes) {
  file_desc* f = state_fd(st(), fd);
  if (!f) return E_BADFD;

  if (!buf) return 0;
  if (!nbytes) return 0;

  if (!(f->flags & _O_RDONLY)) {
    return E_CANT;
  }

  int i = Read(f->link, f->offset, buf, nbytes);
  f->offset += i;
  return i;
}

int _write(int fd, const void *buf, size_t nbytes) {
  file_desc* f = state_fd(st(), fd);
  if (!f) return E_BADFD;

  if (!buf) return 0;
  if (!nbytes) return 0;

  if (!(f->flags & _O_WRONLY)) {
    return E_CANT;
  }

  int i = Write(f->link, f->offset, buf, nbytes);
  f->offset += i;
  //f->size = (f->size > f->offset) ? f->size : f->offset;
  return i;
}

int _remove(const char* path) {
  char* npath = path_normalize(get_cwd(st()), path);
  assert(path);

  char * parent = path_parent(npath);
  char * name = path_name(npath);
  free(npath);

  Node * n = walk(parent);
  free(parent);

  if (!n) return E_NOTFOUND;
  if (!is_dir(n)) return E_INVAL;

  int r = dir_remove(get_dir(n), name);
  free(name);
  return r;
}
