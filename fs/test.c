#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"



/*
void mk(Node *n, int argc, char** argv) {
  assert(argc >= 4);

  switch (*argv[3]) {
    case 'D':
      LINK(n->dir, argv[2], ctor_directory());
      break;
    case 'B':
      LINK(n->dir, argv[2], ctor_block(5));
      break;
    default:
      assert(0);
  }
}

void rm(Node *n, int argc, char** argv) {
  assert(argc >= 3);
  unlink(n->dir, argv[2]);
}
*/

int main(int argc, char** argv) {
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
