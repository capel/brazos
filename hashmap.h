#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"
#include "malloc.h"

// Basic hashmap data structure. It is designed to do a few very
// specific cases extremely fast and well.

// Since arm doesn't have a division or modulo instruction,
// we force table size to be a power of two so we can
// quickly mask.

// It's your problem to make a hash value for the key (or just pass in an integer directly if you want the identity hash and have an integer.

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
    size_t bucket_mask; // the mask to use to find the modulo
    bucket** buckets;
    const alloc_funcs* funcs;
} hashmap;

// power2_num_buckets is the log2 of the number of buckets you want
// eg, 1024 bucket -> 10
// Powers over 31 are not supported.
hashmap* make_hashmap(size_t power2_num_buckets, const alloc_funcs * func);

// The key is assumed to already be hashed.
void hm_insert(hashmap* map, unsigned key, void* val);

// Returns 0 if the key is not found. Store 0 at your own risk.
void* hm_lookup(hashmap* map, unsigned key);

// The data is returned if the key is found, otherwise 0 is returned.
// It is your job to free the data, if necessary.
void* hm_delete(hashmap* map, unsigned key);

// This is somewhat silly since it's specificly for the block cache,
// but its better than that code breaking into our data structure manually.
// or doing some other crazy hack.
void* hm_delete_random(hashmap* map);

#endif
