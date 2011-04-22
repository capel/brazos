#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"

typedef struct _bucket {
    unsigned key;
    void* data;
    struct _bucket *next;
} bucket;

typedef struct _hashmap {
    size_t num_buckets;
    size_t filled;
    bucket* buckets;
} hashmap;

hashmap* make_hashmap(size_t num_buckets);
void hm_insert(hashmap* map, unsigned key, void* val);
void* hm_lookup(hashmap* map, unsigned key);
void hm_delete(hashmap* map, unsigned key);


#endif
