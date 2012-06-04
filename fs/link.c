#include "fs.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <extras.h>

static Node* resolve(Link* l) {
  return walk(l);
}

Link* link_ctor(const char* s) {
  return s;
}

void link_dtor(Link * link) {
  free((char*)link);
}

int link_sync(Link* l) {
  return 0;
}

char* link_serialize(Link * l) {
  int needed = strlen("L('')" + strlen(l) + 1);
  printk("%d", needed);
  char *s = malloc(needed);
  snprintf(s, needed, "L('%s')", l);
  return s;
}

int link_write(Link* b, size_t pos, const void *buf, size_t nbytes) {
  Node* n = resolve(b);
  assert(n);
  return Write(n, pos, buf, nbytes);
}

int link_read(Link* b, size_t pos, void *buf, size_t nbytes) {
  Node* n = resolve(b);
  assert(n);
  return Read(n, pos, buf, nbytes);
}
