#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>

int echo_main(int argc, char** argv) {
  if (argc == 1) {
    printf("\n");
    return 0;
  }
  printf("%s\n", argv[1]);
  return 0;
}
