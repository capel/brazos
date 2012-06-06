
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <extras.h>

const char * path_name(const char * path) {
  size_t len = strlen(path);

  // special case for "/"
  if (len == 1 && path[0] == '/') return path;
  for(int i = len - 1; i >= 0; i--) {
    if (path[i] == '/') {
      return path + i + 1;
    }
  }

  // There is only the filename in the path, return it.
  return path;
}

const char* path_parent(const char * path)  {
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
  
const char * path_normalize(const char * cwd, const char * path) {
  if (!path || !cwd) return 0;

  if (path[0] == '/') return strclone("/");

  if (!strcmp(path, ".")) {
    return strclone(cwd);
  }

  char * s = malloc(strlen(cwd) + strlen(path) + 1);
  strcpy(s, cwd);
  strcat(s, path);
  return s;
}
