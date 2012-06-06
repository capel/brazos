#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>

int mkdir_main(int argc, char** argv) {
  char * path;
  if (argc == 1) {
    printf("mkdir: too few arguments\n");
    return -1;
  }
  path = argv[1];

  int fd = _opendir(path, _O_RDONLY);
  if (fd == E_NOTFOUND) {
    fd = _opendir(path, _O_CREAT);
    assert(fd > 0);
    int r = _closedir(fd);
    assert(r == 0);
    return 0;
  } else if (fd == E_INVAL) {
    printf("mkdir: file %s already exists\n", path);
    return -1;
  } else {
    _closedir(fd);
    printf("mkdir: directory %s already exists\n", path);
    return -1;
  }
}
