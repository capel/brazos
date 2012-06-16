#ifndef PATH_UTIL_H
#define PATH_UTIL_H


char * path_normalize(const char * cwd, const char * path);
char * path_name(const char * path);
char* path_parent(const char * path);

#endif
