#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>

// Basic hashmap data structure. It's your problem to make a hash value
// for the key (or just pass in an integer directly if you want the identity
// hash and have an integer.

// NOTE:
// This table does *not* resize, intentionally. It is meant to be
// initialized to the correct size for its use and not grow. 
// eg: a page cache with N pages really only needs a hashmap of
// vaguely the same size.
// So pick a good size :)

// Also note that lookup returns 0 on not finding the key. If you are
// storing a zero, have fun.

typedef struct _bucket {
    unsigned key;
    void* val;
    struct _bucket *next;
} bucket;

typedef struct _hashmap {
    size_t num_buckets;
    size_t filled;
    bucket** buckets;
} hashmap;

hashmap* make_hashmap(size_t num_buckets);
void hm_insert(hashmap* map, unsigned key, void* val);
void* hm_lookup(hashmap* map, unsigned key);
void hm_delete(hashmap* map, unsigned key);


#endif
