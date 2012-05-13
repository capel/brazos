#ifndef PARSE_DIR_H
#define PARSE_DIR_H

#include "types.h"

typedef struct {
  char name[64];
  int rid;
  int type;
} dir_entry;

typedef struct {
  size_t size;
  dir_entry** entries;
} parsed_dir;

parsed_dir* parse_dir(const char* s);
void cleanup_parsed_dir(parsed_dir* d);

#endif
