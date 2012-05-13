#include "fs.h"
#include <assert.h>
#include <string.h>

int dir_slots_free(Directory* dir) {
  size_t n = 0;
  for(int i = 0; i < ENTRIES_PER_DIR; i++) {
    if (!dir->entries[i]) {
      n++;
    }
  }
  return n;
}

Node* dir_lookup(Directory* dir, const char* name) {
  for(int i = 0; i < ENTRIES_PER_DIR; i++) {
    if (dir->entries[i] && !strcmp(dir->entries[i]->name, name)) {
      return dir->entries[i]->n;
    }
  }
  return 0;
}

int dir_add(Directory* dir, const char* name, Node* n) {
  if (dir_slots_free(dir) == 0) {
    return E_FULL;
  }

  if (dir_lookup(dir, name)) {
    return E_EXISTS;
  }

  for(int i = 0; i < ENTRIES_PER_DIR; i++) {
    if (dir->entries[i] == 0) {
      char * s = malloc(strlen(name));
      strncpy(s, name, NAME_LEN);
      dir->entries[i] = ctor_entry(NODE(s), n);
      return SUCCESS;
    }
  }

  assert(0);
}

int dir_remove(Directory* dir, const char* name) {
  if (!dir_lookup(dir, name)) {
    return E_NOTFOUND;
  }

  for(int i = 0; i < ENTRIES_PER_DIR; i++) {
    if (dir->entries[i] && !strcmp(dir->entries[i]->name, name)) {
      DTOR(dir->entries[i]);
      dir->entries[i] = 0;
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

  if (dir_slots_free(dst) == 0) {
    return E_FULL;
  }

  dir_add(dst, name, n);

  for(size_t i = 0; i < ENTRIES_PER_DIR; i++) {
    if (src->entries[i] && !strcmp(src->entries[i]->name, name)) {
      src->entries[i] = 0;
      return SUCCESS;
    }
  }

  assert(0);
}
