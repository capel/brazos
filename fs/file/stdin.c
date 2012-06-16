#include "../fs.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <extras.h>
#include <unistd.h>

#include "file.h"


static int _size(File* f) {
  return 0;
}

static void _dtor(File* f) {}

static int _read(File* f, size_t pos, void *buf, size_t nbytes) {
  return read(0, buf, nbytes);
}

static int _sync(File* f) { return 0; }

static char* _serialize(File* f) {
  return strclone("S(0)");
}

static special ops = {
  .write = 0, 
  .read = _read,
  .sync = _sync, 
  .size = _size, 
  .dtor = _dtor, 
  .serialize = _serialize,
  .type = STYPE_STDIN
};


File* stdin_ctor() {
  File * f = raw_file_ctor();
  f->s = &ops;
  return f;
}
