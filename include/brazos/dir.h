#ifndef DIRENT_H
#define DIRENT_H

#define NAME_LEN 8


#define _TYPE_DIR 0
#define _TYPE_LINK 1
#define _TYPE_FILE 2

typedef struct _stat_entry { 
  char name[NAME_LEN]; 
  int type;
  size_t size;
} _stat_entry;

int _stat(int fd, struct _stat_entry* out);

int _opendir(const char* path, int flags);
int _nextfile(int dir_fd, struct _stat_entry* out);
int _closedir(int dir_fd);

int _chdir(const char* path);

#endif
