#include "hashmap.h"
//#include "malloc.h"
#include <stdlib.h>
#include <stdio.h>


hashmap* make_hashmap(size_t num_buckets) {
    hashmap* map = (hashmap*) malloc(sizeof(hashmap));
    map->num_buckets = num_buckets;
    map->filled = 0;
    map->buckets = (bucket**) calloc(sizeof(bucket), num_buckets);

    return map;
}

void hm_insert(hashmap* map, unsigned key, void* val) {
    unsigned b = key % map->num_buckets;

    bucket * buck = (bucket*)malloc(sizeof(bucket));
    buck->key = key;
    buck->val = val;
    buck->next = map->buckets[b];
    map->buckets[b] = buck;
}

void* hm_lookup(hashmap* map, unsigned key) {
    unsigned b = key % map->num_buckets;
    
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            return buck->val;
        }
    }

    return 0;
}

void hm_delete(hashmap* map, unsigned key) {
    unsigned b = hash(key) % map->num_buckets;

    bucket * prev = 0;
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            if (prev) {
                prev->next = buck->next;
            } else {
                map->buckets[b] = buck->next;
            }
            free(buck);
        }
    }
}
