#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
#include <file.h>

int cat_main(int argc, char** argv);

void file_init();
void file_shutdown();

int main(int argc, char** argv) {
  file_init();

  cat_main(argc, argv);
  return 0;
  
  /*
  char * o = "D(E('a',1),E('b',B(1)),)";
  //Node * n = parse("D(E('a', 1),E('b',1))");
  Node *n = parse(o);
  char * s = serialize(n);
  printf("%s \n %s", o, s);
  */


  /*
  {
    char * s;
    {
      FILE * f = fopen("data.dir", "r");
      fseek(f, 0, SEEK_END);
      long len = ftell(f);
      fseek(f, 0, SEEK_SET);

      s = malloc(len);
      fread(s, len, 1, f);
      fclose(f);
    }

    {
      Node * n = parse(s);
      pretty_print(n, 0);

      Directory* dir = n->e->n->dir; // heh
      Directory* dir2 = dir_lookup(dir, "dir")->dir;

      dir_add(dir, "cool", NODE(5));
      pretty_print(n, 0);

      dir_remove(dir, "cool");
      pretty_print(n, 0);

      dir_move(dir, dir2, "b");
      pretty_print(n, 0);
    }

    free(s);
  }

  */
  
  int fd = _open("doom", _O_CREAT | _O_RDWR);
  int r = _write(fd, "hey", strlen("hey") + 1);
  assert(r > 0);

  _sync(fd);

  char buf2[PAGE_SIZE + 1];
  memset(buf2, 0, PAGE_SIZE + 1);
  r = _seek(fd, 0, _SEEK_SET);
  assert(r >= 0);
  r = _read(fd, buf2, PAGE_SIZE);
  assert(r > 0);
  assert(!strncmp("hey", buf2, strlen("hey")));
  printf("Hey: %s\n", buf2);

  file_shutdown();

/*
  if (0 == strcmp("mk", argv[1])) {
    mk(n, argc, argv);
  } else if (00 == strcmp("rm", argv[1])) {
    rm(n, argc, argv);
  } else if (0 == strcmp("pr", argv[1])) {
    return 0;
  } else {
    assert(0);
  }
*/

  /*
  s = serialize(n);
  pretty_print(n, 0);

  FILE * f = fopen("data.dir", "w");
  fwrite(s, strlen(s), 1, f);
  fclose(f);
  */
}
