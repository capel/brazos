#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>

int rm_main(int argc, char** argv) {
  if (argc == 1) {
    printf("rm: too few arguments given\n");
    return -1;
  }

  int r = _remove(argv[1]);
  if (r == E_NOTFOUND) {
    printf("rm: %s not found\n", argv[1]);
    return -1;
  } else if (r == E_INVAL) {
    printf("rm: invalid argument\n");
    return -1;
  }
  assert(r == 0);

  return 0;
}
