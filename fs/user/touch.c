#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <extras.h>

int touch_main(int argc, char** argv) {
  if (argc == 1) {
    Printf("touch: too few arguments");
    return -1;
  }

  int fd = _open(argv[1], _O_CREAT | _O_RDONLY);
  if (fd == E_INVAL) {
    Printf("touch: invalid argument\n");
    return -1;
  } else if (fd == E_NOTFOUND) {
    Printf("touch: parent directory does not exist\n");
    return -1;
  }

  assert(fd > 0);

  int r = _close(fd);
  assert(r == 0);

  return 0;
}
