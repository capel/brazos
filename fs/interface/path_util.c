
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <extras.h>

char * path_name(const char * path) {
  size_t len = strlen(path);

  // special case for "/"
  if (len == 1 && path[0] == '/') return strclone(path);
  for(int i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      return strclone(path + i + 1);
    }
  }

  // There is only the filename in the path, return it.
  return strclone(path);
}

char* path_parent(const char * path)  {
  size_t len = strlen(path);
  char * s = malloc(len + 1);
  strcpy(s, path);

  // special case for "/"
  if (len == 1 && path[0] == '/') return s;

  for(int i = len - 1; i >= 0; i--) {
    if (s[i] == '/') {
      if (i == 0) {
        s[1] = '\0';
        return s;
      } else {
        s[i] = '\0';
        return s;
      }
    }
  }
  assert(0);
}
  
char * path_normalize(const char * cwd, const char * path) {
  assert(path && cwd);

  if (path[0] == '/') {
    return strclone(path);
  }

  if (!strcmp(path, ".")) {
    return strclone(cwd);
  }

  char * s = malloc(strlen(cwd)  + 1 + strlen(path) + 1);
  strcpy(s, cwd);
  if (s[strlen(s) - 1] != '/') strcat(s, "/");
  strcat(s, path);
  return s;
}
