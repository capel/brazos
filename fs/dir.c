#include "fs.h"
#include <assert.h>
#include <string.h>
#include "../vector.h"

struct Entry {
  char name[NAME_LEN];
  Node* n;
};

struct Directory { 
  Block * b;
  vector* v;
};

Entry* ctor_entry(const char* name, Node* n) {
  Entry* e = malloc(sizeof(Entry));
  strncpy(e->name, name, NAME_LEN);
  e->n = n;

  return e;
}

void dtor_entry(Entry *e) {
  DTOR(e->n);
  free(e);
}


static Directory* _root = 0;

void set_root(Directory* root) {
  assert(!_root && root);
  _root = root;
}

Directory* ctor_directory(Block * b) {
  Directory* d = malloc(sizeof(Directory));
  d->b = b;
  d->v = make_vector(8);
  return d;
}

void dtor_directory(Directory* dir) {
  foreach(Entry*,e,idx,dir->v) {
    DTOR(e);
  }
  DTOR(dir->b);
  free(dir);
}

int dir_read(Directory* d, size_t pos, void *buf, size_t nbytes) {
  dir_sync(d);
  return block_read(d->b, pos, buf, nbytes);
}

Node* walk(const char* path) {
  Directory* start = _root;
  Node * o;
  vector* v = ksplit_to_vector(path, "/");
  for (size_t i = 0; i < v->size; i++) {
    o = dir_lookup(start, v->data[i]);
    if (!o) {
      cleanup_vector(v);
      return 0;
    }

    if (i+1 == v->size) {
      cleanup_vector(v);
      return o;
    }

    if (o->type == DIRECTORY) {
      start = o->dir;
    } else {
      cleanup_vector(v);
      return 0;
    }
  }

  cleanup_vector(v);
  return o; 
}

Node* dir_lookup(Directory* dir, const char* name) {
  foreach(Entry*, e, i, dir->v) {
    assert(e);
    if (!strcmp(e->name, name)) {
      return e->n;
    }
  }
  return 0;
}

int dir_add(Directory* dir, const char* name, Node* n) {
  if (dir_lookup(dir, name)) {
    return E_EXISTS;
  }

  vector_push(dir->v, (char*)ctor_entry(name, n));
  return 0;
}

int dir_remove(Directory* dir, const char* name) {
  if (!dir_lookup(dir, name)) {
    return E_NOTFOUND;
  }

  foreach(Entry*, e, idx, dir->v) {
    assert(e);
    if (!strcmp(e->name, name)) {
      DTOR(e);
      vector_remove(dir->v, idx);
      return SUCCESS;
    }
  }

  assert(0);
}

int dir_move(Directory* src, Directory* dst, const char* name) {
  Node * n = dir_lookup(src, name);
  if (!n) {
    return E_NOTFOUND;
  }

  if (dir_lookup(dst, name)) {
    return E_EXISTS;
  }

  dir_add(dst, name, n);

  foreach(Entry*, e, idx, dst->v) {
    assert(e);
    if (!strcmp(e->name, name)) {
      vector_remove(dst->v, idx);
      return SUCCESS;
    }
  }

  assert(0);
}
