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

void set_root(Directory* root);

int valid(int off) {
  if (off < 0) return 0;
  if (off > PAGE_SIZE) return PAGE_SIZE - 1;
  return off;
}

static int bid = 1;
int allocate_bid() {
  return bid++;
}

typedef struct file_desc {
  Link* link;
  size_t offset;
  int flags;
} file_desc;

static file_desc* fd_table[MAX_FDS];

void file_shutdown() {
  for(size_t i = 0; i < MAX_FDS; i++) {
    if (fd_table[i]) {
      _sync(i);
      _close(i);
    }
  }
  
  printk("shutdown");
}

static int allocate_fd() {
  for (int i = 1; i < MAX_FDS; i++) {
    if (fd_table[i] == 0) {
      return i;
    }
  }
  return E_FULL;
}


static const char * _cwd = 0;
const char * get_cwd() {
  return _cwd;
}

void set_cwd(const char* cwd) {
  assert(cwd);
  if (_cwd) free((char*)_cwd);

  _cwd = strclone(cwd);
}

int _open(const char *orig_path, int flags) {
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
      n = NODE(file_ctor(0,0,0));
      assert(n);

      const char * parent = path_parent(path);
      const char * name = path_name(path);

      Node * parent_dir = walk(parent);
      free((char*)parent);

      Directory* dir = get_dir(parent_dir);
      dir_add(dir, name, n);
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

  fd_table[fd] = f;

  free((char*) path);
  return fd;
}

int _close(int fd) {
  if (fd > MAX_FDS) return E_BADFD;

  file_desc* f = fd_table[fd];
  if (!f) return E_BADFD;

  Sync(f->link);

  DTOR(f->link);

  free(f);
  fd_table[fd] = 0;
  return 0;
}

int _sync(int fd) {
  if (fd > MAX_FDS) return E_BADFD;

  file_desc* f = fd_table[fd];
  if (!f) return E_BADFD;

  Sync(f->link);
  return 0;
}

int _seek(int fd, int offset, int whence) {
  if (fd > MAX_FDS) return E_BADFD;

  file_desc* f = fd_table[fd];
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
  if (fd > MAX_FDS) return E_BADFD;
  file_desc* f = fd_table[fd];
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
  if (fd > MAX_FDS) return E_BADFD;
  file_desc* f = fd_table[fd];
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

int _stat(int fd) {
  if (fd > MAX_FDS) return E_BADFD;
  file_desc* f = fd_table[fd];
  if (!f) return E_BADFD;

  return PAGE_SIZE; //f->size;
}

int _remove(const char* path) {
  const char* npath = path_normalize(get_cwd(), path);
  assert(path);

  const char * parent = path_parent(npath);
  const char * name = path_name(npath);

  Node * n = walk(parent);
  free((char*)parent);
  free((char*)npath);

  Directory* dir = get_dir(n);
  return dir_remove(dir, name);
}
