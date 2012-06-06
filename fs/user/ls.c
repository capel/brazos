#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>

int ls_main(int argc, char** argv) {
  char * path;
  if (argc == 1) {
    path = ".";
  } else {
    path = argv[1];
  }

  int fd = _opendir(path, _O_RDONLY);
  if (fd == E_NOTFOUND) {
    printf("ls: %s not found\n", path);
    return -1;
  } else if (fd == E_INVAL) {
    printf("ls: %s not a directory\n", path);
  }
  assert(fd > 0);

  char buf[32];
  int r = 0;

  while((r = _nextfile(fd, buf, 32)) == 0) {
    printf("%s\n", buf);
  }

  r = _closedir(fd);
  assert(r == 0);

  return 0;
}
