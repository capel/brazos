#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <file.h>
#include <assert.h>

#include <bcurses.h>
#include <extras.h>

int vim_main(int argc, char** argv) {
  if (argc == 1) {
    Printf("vim: No arguments supplied\n");
    return -1;
  }
  char * path = argv[1];

  int fd = _open(path, _O_RDONLY);
  if (fd == E_NOTFOUND) {
    Printf("vim: %s not found\n", path);
    return -1;
  } else if (fd == E_INVAL) {
    Printf("vim: %s is a directroy\n", path);
    return -1;
  }
  assert(fd > 0);

  int size = 4096; // heh. FIXME
  assert(size > 0);

  char * buf = malloc(size + 1);
  int r = _read(fd, buf, size);
  if (r == E_CANT) {
    Printf("vim: no permission to read %s", path);
    free(buf);
    return -1;
  }
  assert(r > 0);

  scr* sc = init_screen(50, 10);
  canvas* cur = full_canvas(sc);
  textbox* tb = init_textbox(cur, buf);

  int ch;
  blit(sc);
  for (;;) {
    ch = readch();
    if (is_magic(ch)) {
      standard_magic(tb, ch);
    } else if (ch == '!') {
      goto end;
    } else {
      insert(tb, ch); 
    }
    blit(sc);
  }

end:

  get_text(tb, buf, 4096);

  dtor_screen(sc);

  r = _close(fd);
  assert(r == 0);

  r = _remove(path);
  assert(r == 0);

  fd = _open(path, _O_CREAT | _O_WRONLY);
  assert(fd >= 0);

  r = _write(fd, buf, strlen(buf));
  assert(r >= 0);

  r = _close(fd);
  assert(r == 0);

  free(buf);

  return 0;
}
