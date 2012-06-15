#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>

int echo_main(int argc, char** argv) {
  for(int i = 1; i < argc; i++) {
    printf("%s", argv[i]);
    if (i != argc-1) {
      printf(" ");
    }
  }
  printf("\n");
  return 0;
}
