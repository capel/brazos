#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
#include <file.h>
#include <vector.h>

int sh_main(int argc, char** argv);

void root_init();
void root_shutdown();
void file_shutdown();
void fs_init();
void blocks_init();
void blocks_shutdown();

void set_cwd(const char* s);

int main0(int argc, char** argv) { 
  vector * v = make_vector(8);
  push(v, -1);
  push(v, 2);
  push(v, 3);
  push(v, 5);

  printf("orig: ");
  each(int, x, v, printf("%d ", x));
  printf("\n");

  printf("sum: %d\n", sum(int, v));
  printf("min: %d\n", min(int, v));
  printf("max: %d\n", max(int, v));

  printf("find: %d\n", find(int, x, v, x == 3));
  printf("reduce: %d\n", reduce(int, x, v, o, o * x));

  printf("orig: ");
  each(int, x, v, printf("%d ", x));
  printf("\n");

  {
    vector *m = make_vector(vsize(v));
    //each(int, x, v, vector_push(m,(char*) (x*3)));

    /*
    printf("map: "); 
    each(int, x, m, printf("%d ", x));
    printf("\n");
    */
    
    cleanup_vector(m);
  }

  each(int, x, v, printf("%d ", x));
  printf("\n");
  printf("take: %d\n", take(int, x, v, x == 2));
  each(int, x, v, printf("%d ", x));
  printf("\n");

  {
    vector * f = filter(int, x, v, x > 0);

    /*
    printf("filter: "); 
    each(int, x, f, printf("%d ", x));
    printf("\n");
    */

    cleanup_vector(f);
  }

  printf("orig: ");
  each(int, x, v, printf("%d ", x));
  printf("\n");
  printf("contains: %d\n", contains(int, v, 5));
  printf("all: %d\n", all(int, x, v, x > 0));
  printf("any: %d\n", any(int, x, v, x > 0));


  cleanup_vector(v);

  return 0;
}

int main3() {
  vector* v = make_vector(8);
  cleanup_vector(v);
  v = 0;
  return 0;
}




int main(int argc, char** argv) {
  blocks_init();
  root_init();
  fs_init();

  /*
  int fd = _open("/woot", _O_CREAT);
  assert(fd > 0);
  _close(fd);
  */
  sh_main(argc, argv);

  file_shutdown();
  root_shutdown();
  blocks_shutdown();
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
  
  /*
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
  root_shutdown();
*/
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
