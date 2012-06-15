#ifndef INTERFACE_COMMON_H
#define INTERFACE_COMMON_H


#define MAX_FDS 64
#include "fs.h"

typedef struct file_desc file_desc;
typedef struct dir_desc dir_desc;

fs_state* ctor_state();

file_desc* state_fd(fs_state* st, int i);
dir_desc* state_dd(fs_state* st, int i);

int state_ctor_fd(fs_state* st, file_desc* f);
int state_ctor_dd(fs_state* st, dir_desc* d);

void state_dtor_fd(fs_state* st, int fd);
void state_dtor_dd(fs_state* st, int dd);

const char * get_cwd(fs_state* st);
void set_cwd(fs_state* st, const char* cwd);

#endif
