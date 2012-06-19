#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <assert.h>
#include <dir.h>
#include <extras.h>

int echo_main(int argc, char** argv) {
  for(int i = 1; i < argc; i++) {
    Printf("%s", argv[i]);
    if (i != argc-1) {
      Printf(" ");
    }
  }
  Printf("\n");
  return 0;
}
