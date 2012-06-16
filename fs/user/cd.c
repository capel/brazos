#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>

int cd_main(int argc, char** argv) {
  char * path;
  if (argc == 1) {
    path = "/";
  } else {
    path = argv[1];
  }

  int r = _chdir(path);
  if (r == E_NOTFOUND) {
    printf("cd: %s not found\n", path);
    return -1;
  } else if (r == E_INVAL) {
    printf("cd: %s is not a directory\n", path);
    return -1;
  }
  return 0;
}
