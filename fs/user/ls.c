#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>

#include <../../chars.h>

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
    return -1;
  }
  assert(fd > 0);

  struct _stat_entry e;
  int r;

  while((r = _nextfile(fd, &e)) == 0) {
    switch (e.type) {
      case _TYPE_DIR:
        printf("%s", RED);
        break;
      case _TYPE_LINK:
        printf("%s", BLUE);
        break;
      case _TYPE_FILE:
        printf("%s", WHITE);
        break;
    }
    printf("%s%s %lu\n", e.name, WHITE, e.size);
  }

  r = _closedir(fd);
  assert(r == 0);

  return 0;
}
