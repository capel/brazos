#include "hashmap.h"
#include "rand.h"

hashmap* make_hashmap(size_t power2_num_buckets, const alloc_funcs * funcs) {
    if (power2_num_buckets > 31) return 0;

    hashmap* map = (hashmap*) funcs->malloc(sizeof(hashmap));
    map->funcs = funcs;
    map->bucket_mask = (1 << power2_num_buckets) - 1;
    map->buckets = (bucket**) map->funcs->calloc(sizeof(bucket), 1 << power2_num_buckets);

    return map;
}

void hm_insert(hashmap* map, unsigned key, void* val) {
    unsigned b = key & map->bucket_mask;

    bucket * buck = (bucket*)map->funcs->malloc(sizeof(bucket));
    buck->key = key;
    buck->val = val;
    buck->next = map->buckets[b];
    map->buckets[b] = buck;
}

void* hm_lookup(hashmap* map, unsigned key) {
    unsigned b = key & map->bucket_mask;
    
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            return buck->val;
        }
    }

    return 0;
}

void* hm_delete_random(hashmap * map) {
    unsigned b = rand() & map->bucket_mask;
    bucket * buck = map->buckets[b];
    map->buckets[b] = buck->next;
    void* data = buck->val;
    map->funcs->free(buck);
    return data;
}

void* hm_delete(hashmap* map, unsigned key) {
    unsigned b = key & map->bucket_mask;

    bucket * prev = 0;
    for(bucket * buck = map->buckets[b]; buck; buck = buck->next) {
        if (buck->key == key) {
            if (prev) {
                prev->next = buck->next;
            } else {
                map->buckets[b] = buck->next;
            }
            void * data = buck->val;
            map->funcs->free(buck);
            return data;
        }
    }
    return 0;
}
