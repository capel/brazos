#ifndef DIRENT_H
#define DIRENT_H

#define NAME_LEN 8

typedef struct _stat_entry { 
  char name[NAME_LEN]; 
  int type;
  size_t size;
} _stat_entry;

int _stat(int fd, struct _stat_entry* e);

int _opendir(const char* path);
int _nextfile(int dir_fd);
int _closedir(int dir_fd);

#endif
