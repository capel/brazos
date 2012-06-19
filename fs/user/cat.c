#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <extras.h>

int cat_main(int argc, char** argv) {
  char * path;
  if (argc == 1) {
    path = ".";
  } else {
    path = argv[1];
  }

  int fd = _open(path, _O_RDONLY);
  if (fd == E_NOTFOUND) {
    Printf("cat: %s not found\n", path);
    return -1;
  } else if (fd == E_INVAL) {
    Printf("cat: %s is a directroy\n", path);
    return -1;
  }
  assert(fd > 0);

  int size = 4096; // heh. FIXME
  assert(size > 0);

  char * buf = malloc(size + 1);
  int r = _read(fd, buf, size);
  if (r == E_CANT) {
    Printf("cat: no permission to read %s", path);
    free(buf);
    return -1;
  }
  assert(r > 0);

  r = _close(fd);
  assert(r == 0);

  Printf("%s\n", buf);
  free(buf);

  return 0;
}
