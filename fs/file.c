#include <file.h>
#include "fs.h"
#include "io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_FDS ENTRIES_PER_DIR

#include "../chars.h"

#define printk(x, args...) printf(GREEN "pk: " WHITE __FILE__ ":%d  [" LIGHT_BLUE "%s" WHITE "] " x "\n", __LINE__, __func__, ## args)


int valid(int off) {
  if (off < 0) return 0;
  if (off > PAGE_SIZE) return PAGE_SIZE - 1;
  return off;
}

static int bid = 1;
int allocate_bid() {
  return bid++;
}

typedef struct file {
  Link* link;
  size_t offset;
  int flags;
  size_t size;
} File;

static File* fd_table[MAX_FDS];

static Block* superblock;
static Node* _root;

Directory* root() {
  assert(_root);
  return _root->dir;
}

void file_init() {
  char sb[PAGE_SIZE];
  superblock = ctor_block(NODE(0));

  block_read(superblock, 0, sb, PAGE_SIZE);

  _root = parse(sb);
  //pretty_print(_root,0);
  assert(_root->type == DIRECTORY);

  set_cwd("/");
}

void file_shutdown() {
  for(size_t i = 0; i < MAX_FDS; i++) {
    if (fd_table[i]) {
      _sync(i);
      _close(i);
    }
  }
  
  printk("shutdown");
  pretty_print(_root, 0);
  
  char * s = serialize_directory(root());
  printf(":: %s\n", s);
  assert(strlen(s) < PAGE_SIZE);

  block_write(superblock, 0, s, strlen(s) + 1);
  block_sync(superblock);

  free(s);
  DTOR(superblock);
  DTOR(root());
}

int allocate_fd() {
  for (int i = 1; i < MAX_FDS; i++) {
    if (fd_table[i] == 0) {
      return i;
    }
  }
  return E_FULL;
}


static char * _cwd = 0;
const char * get_cwd() {
  return _cwd;
}

void set_cwd(const char* cwd) {
  assert(cwd);
  if (_cwd) free(_cwd);

  _cwd = malloc(strlen(cwd) + 1);
  strcpy(_cwd, cwd);
}

const char * path_normalize(const char * cwd, const char* path);

int _open(const char *orig_path, int flags) {
  int fd = allocate_fd();
  if (fd == E_FULL) return E_FULL;

  // the new value must be freed
  // we can just ignore the old value
  const char* path = path_normalize(get_cwd(), orig_path);
  assert(path);

  Node * n = walk(root(), path);
  if (!n) {
    if (!(flags & _O_CREAT)) {
      return E_NOTFOUND;
    } else {
      Node* b = NODE(ctor_block(NODE(allocate_bid())));
      const char * parent = path_parent(path);
      const char * name = path_name(path);
      if (!strcmp(parent, ".")) {
        dir_add(root(), name, b);
        free((char*)parent);
      } else {
        Node * n = walk(root(), parent);
        free((char*)parent);

        assert(n->type == DIRECTORY);

        dir_add(n->dir, name, b);
      }
    }
  }

  free((char*)path);

  File * f = malloc(sizeof(File));
  char * s = malloc(strlen(path) + 1);
  strcpy(s, path);
  f->link = ctor_link(NODE(s));

  f->offset = 0;
  f->size = 0;
  f->flags = flags & (_O_RDWR); // mask out O_CREAT

  fd_table[fd] = f;
  return fd;
}

int _close(int fd) {
  if (fd > MAX_FDS) return E_BADFD;

  File* f = fd_table[fd];
  if (!f) return E_BADFD;

  Node * n = link_resolve(f->link);
  assert(n->type == BLOCK);
  block_sync(n->block);

  DTOR(f->link);

  free(f);
  fd_table[fd] = 0;
  return 0;
}

int _sync(int fd) {
  if (fd > MAX_FDS) return E_BADFD;

  File* f = fd_table[fd];
  if (!f) return E_BADFD;

  Node * n = link_resolve(f->link);
  assert(n->type == BLOCK);
  block_sync(n->block);
  return 0;
}

int _seek(int fd, int offset, int whence) {
  if (fd > MAX_FDS) return E_BADFD;

  File* f = fd_table[fd];
  if (!f) return E_BADFD;

  switch (whence) {
    case _SEEK_SET:
      f->offset = valid(offset);
      return f->offset;
    case _SEEK_CUR:
      f->offset = valid(f->offset + offset);
      return f->offset;
    case _SEEK_END:
      f->offset = valid(f->size + offset);
      f->size = f->offset;
      return f->offset;
    default:
      return E_INVAL;
  }
}

int _read(int fd, void *buf, size_t nbyte) {
  if (fd > MAX_FDS) return E_BADFD;
  File* f = fd_table[fd];
  if (!f) return E_BADFD;

  if (!buf) return 0;
  if (!nbyte) return 0;

  if (!(f->flags & _O_RDONLY)) {
    return E_CANT;
  }

  Node * n = link_resolve(f->link);
  assert(n && n->type == BLOCK);
  int i = block_read(n->block, f->offset, buf, nbyte);
  f->offset += i;
  return i;
}

int _write(int fd, const void *buf, size_t nbyte) {
  if (fd > MAX_FDS) return E_BADFD;
  File* f = fd_table[fd];
  if (!f) return E_BADFD;

  if (!buf) return 0;
  if (!nbyte) return 0;

  if (!(f->flags & _O_WRONLY)) {
    return E_CANT;
  }

  Node * n = link_resolve(f->link);
  assert(n && n->type == BLOCK);
  int i = block_write(n->block, f->offset, buf, nbyte);
  f->offset += i;
  f->size = (f->size > f->offset) ? f->size : f->offset;
  return i;
}

int _stat(int fd) {
  if (fd > MAX_FDS) return E_BADFD;
  File* f = fd_table[fd];
  if (!f) return E_BADFD;

  return PAGE_SIZE; //f->size;
}


const char * path_name(const char * path) {
  size_t len = strlen(path);

  // special case for "/"
  if (len == 1 && path[0] == '/') return path;
  for(int i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      return path + i + 1;
    }
  }

  // There is only the filename in the path, return it.
  return path;
}

const char* path_parent(const char * path)  {
  size_t len = strlen(path);

  // special case for "/"
  if (len == 1 && path[0] == '/') return path;

  for(int i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      char * s = malloc(len + 1);
      strncpy(s, path, len + 1);
      s[len] = '\0';
      return s;
    }
  }

  // There is only the filename in the path, return .
  char * s = malloc(2);
  s[0] = '.';
  s[1] = '\0';
  return s;
}
  
const char * path_normalize(const char * cwd, const char * path) {
  if (!path || !cwd) return 0;

  if (path[0] == '/') return path;

  if (!strcmp(cwd, ".")) {
    char * s = malloc(strlen(cwd) + 1);
    strcpy(s, cwd);
    return s;
  }

  char * s = malloc(strlen(cwd) + strlen(path) + 1);
  strcpy(s, cwd);
  strcat(s, path);
  return s;
}
