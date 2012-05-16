#include "fs.h"
#include <assert.h>
#include <string.h>

static Node* resolve(Link* l) {
  return walk(l);
}

Link* ctor_link(const char* s) {
  return s;
}

void dtor_link(Link * link) {
  free((char*)link);
}
