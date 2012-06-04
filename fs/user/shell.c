#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <vector.h>

int cat_main(int argc, char** argv);
int touch_main(int argc, char** argv);
int rm_main(int argc, char** argv);

int sh_dispatch(char* cmd, int argc, char** argv);


#define DISPATCH(name, func) do { if (!strcmp(cmd, name)) { return func(argc, argv); } } while (0)

int sh_dispatch(char* cmd, int argc, char** argv) {
  switch (cmd[0]) {
    case 'c':
      DISPATCH("cat", cat_main);
      goto not_found;
    case 't':
      DISPATCH("touch", touch_main);
      goto not_found;
    case 'r':
      DISPATCH("rm", rm_main);
      goto not_found;
    default:
      goto not_found;
  }

not_found:
  return -1337;
}

#undef DISPATCH

int sh_main(int argc, char** argv) {
  char buf[4096];
  for(;;) {
    printf("brazos> ");
    fflush(stdout);
    memset(buf, 0, sizeof(buf));
    int r = read(0, buf, sizeof(buf));
    assert(r > 0);

    size_t end = strlen(buf);
    if (buf[end-1] == '\n') {
      buf[end-1] = '\0'; // kill newline
    }

    vector* v = vector_split(buf, " ");
    if (!strcmp(v->data[0], "exit")) {
      return 0;
    }

    int code = sh_dispatch(v->data[0], v->size, v->data);
    if (code == -1337) {
      printf("Command not found\n");
    } else {
      printf("--> %d\n", code);
    }
  }
}
