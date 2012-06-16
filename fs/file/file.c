#include <stdlib.h>


#include "file.h"
#include "../fs.h"


File* raw_file_ctor() {
  return malloc(sizeof(File));
}
  
int file_size(File* f) {
  if (!f->s->size) return E_CANT;
  return f->s->size(f);
}

void file_dtor(File* f) {
  if (!f->s->dtor) return;
  f->s->dtor(f);

  free(f);
}

int file_read(File* f, size_t pos, void *buf, size_t nbytes) {
  if (!f->s->read) return E_CANT;
  return f->s->read(f, pos, buf, nbytes);
}

int file_write(File* f, size_t pos, const void *buf, size_t nbytes) {
  if (!f->s->write) return E_CANT;
  return f->s->write(f, pos, buf, nbytes);
}

int file_sync(File* f) {
  if (!f->s->sync) return 0;
  return f->s->sync(f);
}

char* file_serialize(File* f) {
  return f->s->serialize(f);
}

