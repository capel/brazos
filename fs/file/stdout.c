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

static void _dtor(File* f) { fsync(1); }

static int _write(File* f, size_t pos, const void *buf, size_t nbytes) {
  return write(1, buf, nbytes);
}

static int _sync(File* f) { fsync(1); return 0; }

static char* _serialize(File* f) {
  return strclone("S(1)");
}

static special ops = {
  .write = _write, 
  .read = 0,
  .sync = _sync, 
  .size = _size, 
  .dtor = _dtor, 
  .serialize = _serialize,
  .type = STYPE_STDOUT
};


File* stdout_ctor() {
  File * f = raw_file_ctor();
  f->s = &ops;
  return f;
}
