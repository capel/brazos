#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"
#include "string.h"

unsigned int murmur(const char* s, int len, unsigned int seed);

inline unsigned int hash(const char* s) {
  murmur(s, strlen(s), 0);
}

inline unsigned int hash(string s) { 
  return murmur(s, s.size(), 0);
}

inline unsigned int hash(unsigned int i) {
  return i;
}

// Since arm doesn't have a division or modulo instruction,
// we force table size to be a power of two so we can
// quickly mask.
template<typename K, typename V>
class hashmap : public noncopyable {
  hashmap(size_t power2_num_buckets);
  pair<bool, V> get(const K &k);
  void set(const K &k, V v);
  void remove(const K &k);
  void size() const { return m_size; }
  void empty() const { return size() == 0; }
  
  private:
    class Bucket {
      K key;
      V val;
      Bucket *next;
      Bucket(K k, V v, Bucket* next) : key(k), val(v), next(next) {}
    };
    size_t bucket_mask;
    Bucket** buckets;
    size_t m_size;
};

#endif
