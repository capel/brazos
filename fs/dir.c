#include "fs.h"
#include <assert.h>
#include <string.h>
#include "../vector.h"
#include "../chars.h"
#include <stdio.h>

#include <extras.h>

#define PAGE_SIZE 4096

vector* parse_dir_block(const char * s);

struct Entry {
  char name[NAME_LEN];
  Node* n;
};

struct Directory { 
  Block * b;
  vector* v;
};

Entry* entry_ctor(const char* name, Node* n) {
  Entry* e = malloc(sizeof(Entry));
  strncpy(e->name, name, NAME_LEN);
  e->n = n;

  return e;
}

void entry_dtor(Entry *e) {
  DTOR(e->n);
  free(e);
}



static Directory* _root = 0;

void set_root(Directory* root) {
  assert(!_root && root);
  _root = root;
}

void root_init() {
  set_root(dir_ctor(block_ctor(0)));
}

void root_shutdown() {
  Sync(_root);
  DTOR(_root);
  printk("root shutdown");
}

Directory* dir_ctor(Block * b) {
  Directory* d = malloc(sizeof(Directory));
  d->b = b;
  char buf[PAGE_SIZE];
  block_read(d->b, 0, buf, PAGE_SIZE);

  d->v = parse_dir_block(buf);
  return d;
}

void dir_dtor(Directory* dir) {
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
  if (!strcmp(path, "/")) {
    return NODE(_root);
  }

  Directory* start = _root;
  Node * o;
  vector* v = vector_split(path, "/");
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

    if (is_dir(o)) {
      start = get_dir(o);
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

  vector_push(dir->v, (char*)entry_ctor(name, n));
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

char* entry_serialize(Entry * e) {
  char *n = node_serialize(e->n);
  printk("%s", n);

  int needed = 1;
  needed += strlen(n);
  needed += strlen("E('' )");
  needed += strlen(e->name);

  char *s = malloc(needed);
  snprintf(s, needed, "E('%s' %s)", e->name, n);
  free(n);

  return s;
}

char* dir_serialize(Directory* dir) {
  char * s = block_serialize(dir->b);
  char * buf=  malloc(sizeof("D()" + strlen(s) + 1));
  sprintf(buf, "D(%s)", s);
  free(s);
  return buf;
}

static const char* dir_entries_serialize(Directory* d) {
  vector* v = make_vector(d->v->size);

  foreach(Entry*, e, idx, d->v) {
    vector_push(v, entry_serialize(e));
  }

  const char* s = vector_join(v, "\n");

  foreach(char*, s, idx2, v) {
    free(s);
  }

  cleanup_vector(v);
  return s;
}


static void indent_print(int indent) {
  for(int i = 0; i < indent; i++) {
    printf(" ");
  }
}

/*
static void pretty_print_entry(Entry* e, int indent) {
  if (!e) return;
  indent_print(indent);
  printf("%s: ", e->name);
  pretty_print(e->n, indent);
  printf("\n");
}*/


int dir_write(Directory* b, size_t pos, const void *buf, size_t nbytes) {
  return E_CANT;
}
int dir_sync(Directory* d) {
  const char * s = dir_entries_serialize(d);
  Write(d->b, 0, s, strlen(s));
  Sync(d->b);
  return 0;
}
