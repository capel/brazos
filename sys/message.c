#include "ko.h"
#include "../stdlib.h"
#include "../mem.h"

typedef struct {
  file f;
  char* msg;
} msg;


static err_t msg_map(file* f, size_t* out_size, void** out_ptr) {
  msg *m = (msg*)f;
  printk("mapping msg %k with data %s", m, m->msg);
  *out_size = strlen(m->msg);
  *out_ptr = m->msg;
  return 0;
}

static err_t msg_unmap(file* f, void* ptr) {
  return 0;
}

static void msg_cleanup(ko* o) {
  msg * m = (msg*)o;
  kfree(m->msg);
}

static file_vtable msg_vt = {
  .map = msg_map,
  .unmap = msg_unmap,
};

file* mk_msg(const char* m) {
  msg* d = kmalloc(sizeof(msg));
  KO(d)->cleanup = msg_cleanup;
  KO(d)->type = KO_FILE;
  KO(d)->rc = 1;
  FILE(d)->v = &msg_vt;

  d->msg = kstrclone(m);
  return FILE(d);
}

typedef struct {
  file f;
  void* data;
  size_t size;
} file_impl;

static err_t fi_map(file* f, size_t* out_size, void** out_ptr) {
  file_impl *fi = (file_impl*)f;
  *out_size = fi->size;
  *out_ptr = fi->data;
  return 0;
}

static err_t fi_unmap(file* f, void* ptr) {
  return 0;
}

static void fi_cleanup(ko* o) {
  file_impl *fi = (file_impl*)o;
  kfree(fi->data);
}

static file_vtable fi_vt = {
  .map = fi_map,
  .unmap = fi_unmap,
};

file* mk_file(void* ptr, size_t size) {
  file_impl* fi = kmalloc(sizeof(file_impl));
  KO(fi)->cleanup = fi_cleanup;
  KO(fi)->type = KO_FILE;
  KO(fi)->rc = 1;
  FILE(fi)->v = &fi_vt;

  fi->data = kmalloc(size);
  fi->size = size;
  memcpy(fi->data, ptr, size);

  return FILE(fi);
}
