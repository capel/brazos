#include <file.h>
#include <mach/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <extras.h>
#include "common.h"

struct fs_state {
  file_desc* fds[MAX_FDS];
  dir_desc* dds[MAX_FDS];
  const char * cwd;
};

fs_state* ctor_state() {
  fs_state * st = calloc(sizeof(fs_state), 1);
  st->cwd = strclone("/");
  return st;
}

file_desc* state_fd(fs_state* st, int i) {
  if (i < 0 || i >= MAX_FDS) return 0;
  return st->fds[i]; // if its NULL, whatev
}

dir_desc* state_dd(fs_state* st, int i) {
  i -= 1000;
  if (i < 0 || i > MAX_FDS) return 0;
  return st->dds[i]; // if its NULL, whatev
}

int state_ctor_fd(fs_state* st, file_desc* f) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (st->fds[i] == 0) {
      st->fds[i] = f;
      return i;
    }
  }
  assert(0);
}

void state_dtor_fd(fs_state* st, int fd) {
  assert(fd >= 0 && fd < MAX_FDS);
  st->fds[fd] = 0;
}

void state_dtor_dd(fs_state* st, int dd) {
  dd -= 1000;
  assert(dd >= 0 && dd < MAX_FDS);
  st->dds[dd] = 0;
}

int state_ctor_dd(fs_state* st, dir_desc* d) {
  for (int i = 0; i < MAX_FDS; i++) {
    if (st->dds[i] == 0) {
      st->dds[i] = d;
      return i + 1000;
    }
  }
  assert(0);
}

const char * get_cwd(fs_state* st) {
  return st->cwd;
}

void set_cwd(fs_state* st, const char* cwd) {
  assert(cwd);
  free((char*)st->cwd);
  st->cwd = strclone(cwd);
}
