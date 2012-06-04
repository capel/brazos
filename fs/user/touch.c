#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>

int touch_main(int argc, char** argv) {
  if (argc == 1) {
    printf("touch: too few arguments");
    return -1;
  }

  int fd = _open(argv[1], _O_CREAT | _O_RDONLY);
  assert(fd > 0);

  int r = _close(fd);
  assert(r == 0);

  return 0;
}
