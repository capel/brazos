#include "fs.h"
#include <assert.h>
#include <string.h>

#include <vector.h>
#include <stdio.h>

#include <extras.h>

#include <dir.h>

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

int dir_size(Directory* d) {
  return vsize(d->v);
}

Entry* entry_ctor(const char* name, Node* n) {
  Entry* e = malloc(sizeof(Entry));
  strncpy(e->name, name, NAME_LEN);
  e->n = n;

  return e;
}

int entry2stat(Entry* e, struct _stat_entry *out) {
  if (!e) return E_INVAL;

  strncpy(out->name, e->name, NAME_LEN);
  out->type = node_type(e->n);
  out->size = Size(e->n);
  return 0;
}

void entry_dtor(Entry *e) {
  DTOR(e->n);
  free(e);
}


int dir_stat(Directory* dir, int pos, struct _stat_entry* out) {
  if (pos < 0 || (size_t)pos >= vsize(dir->v)) return E_NOTFOUND;

  return entry2stat((Entry*)vget(dir->v, pos), out);
}

static Node* _root = 0;

Node* root() { return _root; }

void root_init() {
  _root = NODE(dir_ctor(block_ctor(0)));
}

void root_shutdown() {
  Sync(_root);
  DTOR(_root);
  printk("root shutdown");
}

Directory* dir_ctor(Block * b) {
  Directory* d = malloc(sizeof(Directory));

  if (b == 0) {
    d->b = block_ctor(bid_alloc());
    d->v = make_vector(8);
    return d;
  }

  d->b = b;
  char buf[PAGE_SIZE];
  block_read(d->b, 0, buf, PAGE_SIZE);

  d->v = parse_dir_block(buf);
  return d;
}

void dir_dtor(Directory* dir) {
  each(Entry*, e, dir->v, DTOR(e));
  cleanup_vector(dir->v);
  DTOR(dir->b);
  free(dir);
}

int dir_read(Directory* d, size_t pos, void *buf, size_t nbytes) {
  dir_sync(d);
  return block_read(d->b, pos, buf, nbytes);
}


Node* walk(const char* path) {
  if (!strcmp(path, "/")) {
    return root();
  }

  Directory* start = get_dir(root());
  Node * o;
  vector* v = vector_split(path, "/");
  fori(i, vsize(v)) {
    o = dir_lookup(start, vcget(v, i));
    if (!o) {
      cleanup_vector(v);
      return 0;
    }

    if (i+1 == vsize(v)) {
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
  size_t i = findi(Entry*, e, dir->v, !strcmp(name, e->name));
  if (i == NOT_FOUND) return 0;
  return ((Entry*)(vget(dir->v, i)))->n;
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

  Entry* e = take(Entry*, e, dir->v, !strcmp(e->name, name));
  DTOR(e);
  return SUCCESS;
}

int dir_move(Directory* src, Directory* dst, const char* name) {
  if (dir_lookup(dst, name)) {
    return E_EXISTS;
  }

  Entry* e = take(Entry*, e, dst->v, !strcmp(e->name, name));
  if (!e) {
    return E_NOTFOUND;
  }
  dir_add(dst, name, e->n);
  return SUCCESS;
}

char* entry_serialize(Entry * e) {
  char *n = node_serialize(e->n);
  printk("%s", n);

  int needed = 3;
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
  char * buf=  malloc(sizeof("D()") + strlen(s) + 2);
  sprintf(buf, "D(%s)", s);
  printk("%s", buf);
  free(s);
  return buf;
}

static const char* dir_entries_serialize(Directory* d) {
  vector* v = map(Entry*, e, d->v, entry_serialize(e));

  const char* s = vector_join(v, "\n");

  each(char*, s, v, free(s));

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
  char buf[PAGE_SIZE];
  memset(buf, 0, PAGE_SIZE);

  const char * s = dir_entries_serialize(d);
  strncpy(buf, s, PAGE_SIZE);
  free((char*)s);

  Write(d->b, 0, buf, PAGE_SIZE);
  Sync(d->b);
  return 0;
}
