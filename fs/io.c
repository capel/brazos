#include "io.h"

#include <stdio.h>
#include <string.h>

void bload(int bid, void* page) {
  char name[64];
  sprintf(name, "%d.block", bid);

  FILE * f = fopen(name, "rb");
  if (!f) {
    memset(page, 0, PAGE_SIZE);
    return;
  }

  fread(page, PAGE_SIZE, 1, f);
  fclose(f);
}

void bsync(int bid, void* page) {
  char name[64];
  sprintf(name, "%d.block", bid);

  FILE * f = fopen(name, "wb");
  fwrite(page, PAGE_SIZE, 1, f);
  fclose(f);
}
