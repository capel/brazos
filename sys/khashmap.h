#ifndef KHASHMAP_H
#define KHASHMAP_H

#include "../types.h"
#include "../mem.h"
#include "ko.h"
#include "../vector.h"

// Basic hashmap data structure. It is designed to do a few very
// specific cases extremely fast and well.

// Since arm doesn't have a division or modulo instruction,
// we force table size to be a power of two so we can
// quickly mask.

// It's your problem to make a hash value for the key (or just pass in an integer 
// directly if you want the identity hash and have an integer.

// NOTE:
// This table does *not* resize, intentionally. It is meant to be
// initialized to the correct size for its use and not grow. 
// eg: a page cache with N pages really only needs a hashmap of
// vaguely the same size.
// So pick a good size :)

// Also note that lookup returns 0 on not finding the key. If you are
// storing a zero, have fun.

typedef struct _bucket {
    const char* key;
    ko * value;
    struct _bucket *next;
} kbucket;

typedef struct _hashmap {
    size_t bucket_mask; // the mask to use to find the modulo
    size_t size;
    kbucket** buckets;
} khashmap;

// power2_num_buckets is the log2 of the number of buckets you want
// eg, 1024 bucket -> 10
// Powers over 31 are not supported.
khashmap* mk_khashmap(size_t power2_num_buckets);

vector* khm_keys(khashmap* map);

// The key is assumed to already be hashed.
// True on insert, false on failure (the key already exists)
bool khm_insert(khashmap* map, const char *key, ko* val);

// Returns 0 if the key is not found. Store 0 at your own risk.
ko* khm_lookup(khashmap* map, const char* key);

// The data is returned if the key is found, otherwise 0 is returned.
// It is your job to free the data, if necessary.
bool khm_delete(khashmap* map, const char* key);

void khm_cleanup(khashmap* map);

#endif
