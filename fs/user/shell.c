#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <vector.h>

#include <file.h>
#include <extras.h>

int cat_main(int argc, char** argv);
int touch_main(int argc, char** argv);
int rm_main(int argc, char** argv);
int ls_main(int argc, char** argv);
int mkdir_main(int argc, char** argv);
int echo_main(int argc, char** argv);
int cd_main(int argc, char** argv);
int vim_main(int argc, char** argv);

int sh_dispatch(char* cmd, int argc, char** argv);


#define DISPATCH(name, func) do { if (!strcmp(cmd, name)) { return func(argc, argv); } } while (0)

int sh_dispatch(char* cmd, int argc, char** argv) {
  switch (cmd[0]) {
    case 'c':
      DISPATCH("cat", cat_main);
      DISPATCH("cd", cd_main);
      goto not_found;
    case 'e':
      DISPATCH("echo", echo_main);
      goto not_found;
    case 't':
      DISPATCH("touch", touch_main);
      goto not_found;
    case 'r':
      DISPATCH("rm", rm_main);
      goto not_found;
    case 'l':
      DISPATCH("ls", ls_main);
      goto not_found;
    case 'v':
      DISPATCH("vim", vim_main);
      goto not_found;
    case 'm':
      DISPATCH("mkdir", mkdir_main);
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
    Printf("brazos> ");
    memset(buf, 0, sizeof(buf));
    int r = _read(0, buf, sizeof(buf));
    assert(r >= 0);

    if (r == 0) continue;

    size_t end = strlen(buf);
    if (buf[end-1] == '\n') {
      buf[end-1] = '\0'; // kill newline
    }

    vector* v = split_quoted(buf, " ");

    if (vsize(v) == 0) {
      cleanup_vector(v);
      continue;
    }

    if (!strcmp(vcget(v, 0), "exit")) {
      cleanup_vector(v);
      return 0;
    }

    int code = sh_dispatch(vcget(v, 0), vsize(v), (char**)vdata(v));
    if (code == -1337) {
      Printf("Command not found\n");
    }
    cleanup_vector(v);
  }
}
