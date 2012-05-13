#ifndef IO_H
#define IO_H

#define PAGE_SIZE 4096

// buf must always be be PAGE_SIZE (4096) byes
void bload(int bid, void* page);
void bsync(int bid, void* page);

#endif
