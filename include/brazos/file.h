#ifndef FILE_H
#define FILE_H

#include <stdlib.h>

#define PAGE_SIZE 4096

#define SUCCESS 0
#define E_EXISTS -50
#define E_NOTFOUND -51
#define E_FULL -52
#define E_INVAL -53
#define E_CANT -54
#define E_BADFD -55

#define _O_RDONLY 0x1
#define _O_WRONLY 0x2
#define _O_RDWR 0x1 | 0x2
#define _O_CREAT 0x4

// Path operations

// Returns the parent in a path (eg, /parent/name -> /parent)
// This returns a new string that must be freed manually
// If the path is relative and contains no parent, it returns "."
// If the path is /, it returns "/"
const char* path_parent(const char * path);

// Returns the filename in a path (eg, /parent/name -> name)
// This does _NOT_ return a new string, only a pointer into the old one
// If the path is /, it returns "/"
const char * path_name(const char * path);

void set_cwd(const char * path);
const char * get_cwd();

// File Operations
// Opens a filename and returns a file handle.
// Supported flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT
// Returns fd > 0 on success,
// E_NOTFOUND if the file was not found and O_CREAT wasn't set
// E_FULL if no FDs are available
int _open(const char *path, int flags);

// Reads data from a file given by fd into a buffer
// If nbytes is 0 or buf is null, no action is taken
// Returns number of byte read (> 0) on success,
// E_BADFD if the fd is bad
// E_CANT if the file is not open for reading
int _read(int fd, void *buf, size_t nbyte);

// Write data from a buffer given into a file given by fd
// If nbytes is 0 or buf is null, no action is taken
// Returns number of byte written (> 0) on success,
// E_BADFD if the fd is bad
// E_CANT if the file is not open for writing
int _write(int fd, const void *buf, size_t nbyte);

#define _SEEK_SET 1
#define _SEEK_CUR 2
#define _SEEK_END 3

// Seeks the offset of the file given by fd
// Supported whences:
// SEEK_SET: set file offset to offset
// SEEK_CUR: set file offset to current offset + offset
// SEEK_END: set file offset to file size + offset
// Returns the resulting offset on success
// E_BADFD if the fd is bad
// E_INVAL if whence isn't supported
int _seek(int fd, int offset, int whence);


// Syncs file to disk.
// Returns 0 on success
// E_BADFD fd is bad
int _sync(int fd);

// Closes a file and syncs it to disk
// Returns 0 on success
// E_BADFD fd is bad
int _close(int fd);

// Removes a file from a directory
// Returns 0 on success
// E_NOTFOUND if the path does not indicate a file
int _remove(const char* path);
int _rename(const char* old, const char* new_path);

#endif
