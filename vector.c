// David Capel <capel@wisc.edu>, cs  account: capel

// Implementation of the auto-expanding array data structure.

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "vector.h"

#include <functional.h>
#include <extras.h>
//
// An auto-expanding array. You must use make_vector(data_type_size) to make it 
// and vector_push to add items to it. When you are finished, call cleanup_vector.
// data_type_size is the size of the data type you are storing in the array.
// example: vector* v = make_vector(sizeof(whatever*));
struct vector {
  // Public data -- use these and the provided vector functions.
  void** data;
  size_t size;

  // Private data -- do not use them.
  size_t __allocated_size;
  // This is used for the split_to_vector function.
  char* __source;
};

void* vector_pop_front(vector* v)
{
  void* p = (void*)v->data[0];
  vremove(v, 0);
  return p;
}

size_t vsize(vector* v) {
  assert(v);
  return v->size;
}

void vpush(vector* v, void* object)
{
  assert(v);

  if (v->size >= v->__allocated_size-2) {
    size_t old_size = v->__allocated_size;
    v->__allocated_size *= 2;
    void* old = v->data;
    v->data = malloc(v->__allocated_size * sizeof(char*));
    memcpy(v->data, old, old_size * sizeof(char*));

    free(old);
  }

  v->data[v->size] = object;
  ++v->size;
}

void* vget(vector* v, size_t idx) {
  assert(v);
  assert(idx < v->size);
  return v->data[idx];
}

void** vdata(vector* v) {
  return v->data;
}

char* vcget(vector* v, size_t idx) {
  return vget(v, idx);
}

vector* make_vector(int init_size)
{
  vector* v = malloc(sizeof(vector));
  v->__allocated_size = MAX(init_size, 8);
  v->data = malloc(v->__allocated_size * sizeof(char*));
  v->size = 0;
  v->__source = 0;
  return v;
}

void cleanup_vector(vector* v)
{
  if (!v)
    return;

  free(v->__source);
  free(v->data);
  free(v);
}

void vinsert(vector* v, size_t idx, void* item) {
  vpush(v, 0); // so we have enough space

  for(size_t i = vsize(v) - 1; i != idx; i--) {
    v->data[i] = vget(v, i-1);
  }

  v->data[idx] = item;
}

void* vremove(vector* v, size_t i)
{
  if (i >= v->size)
    return 0;

  if (!v)
    return 0;

  void* item = v->data[i];

  // copy stuff over
  for(size_t j = i+1; j < v->size; ++j) {
    v->data[j-1] = v->data[j];
  }


  // shorten the vector
  v->data[v->size-1] = 0;
  --v->size;

  return item;
}


bool is_sep(const char c, const char* seps)
{
  if (!c) return true;
  size_t len = strlen(seps);
  for (size_t i = 0; i < len; ++i) {
    if (c == seps[i])
      return true;
  }
  return false;
}

vector* split(const char * str, const char* seps)
{

  if (!str || !seps)
    return 0;

  vector * v = make_vector(8);

  size_t len = strlen(str);
  v->__source = malloc(len+2);
  strlcpy(v->__source, str, len+1);

  // for ease of use
  char* src = v->__source;

  size_t i = 0;
  size_t beginning;


  while (true) {
    // skip beginning stuff
    for(; i < len; ++i) {
      if (!is_sep(src[i], seps)) {
        break;
      }
    }

    if (i >= len) {
      return v;
    }

    beginning = i;
    for (; i < len; ++i) {
      if (is_sep(src[i], seps)) {
        src[i++] = '\0';
        vpush(v, src+beginning);
        break;
      }
    }


    if (i == len) {
      if (i != beginning) {
        src[++i] = '\0';
        vpush(v, src+beginning);
      }
      return v;
    }
  }
}

static bool is_quote(char c) {
  return (!c || c == '\'' || c == '"');
}

static void add(vector* v, char* src, size_t i, char* beginnning) {
  src[i] = '\0';
  vpush(v, beginnning);
}

static void get_str(vector*v, char* s, size_t *i) {
  assert(is_quote(s[*i]));
  *i += 1;

  char* beginning = s + *i; 

  while(!is_quote(s[*i])) { *i += 1; }
  assert(is_quote(s[*i]));
  
  add(v, s, *i, beginning);
  *i += 1;
}

static void eat_seps(char* s, size_t *i, const char* seps) {
  while(s[*i] && is_sep(s[*i], seps)) { *i += 1; }
}

static void get_token(vector*v, char* s, size_t *i, const char* seps) {
  char* beginning = s + *i; 

  while(!is_sep(s[*i], seps)) { *i += 1; }
  assert(is_sep(s[*i], seps));
  
  add(v, s, *i, beginning);
  *i += 1;
}

vector* split_quoted(const char * str, const char* seps)
{
  if (!str || !seps)
    return 0;

  vector * v = make_vector(8);

  v->__source = strclone(str);
  size_t len = strlen(v->__source);

  // for ease of use
  char* s = v->__source;

  size_t i = 0;

  while (true) {
    if (i >= len) {
      return v;
    }

    eat_seps(s, &i, seps);

    if (i >= len) {
      return v;
    }


    if (is_quote(s[i])) {
      get_str(v, s, &i);
    } else {
      get_token(v, s, &i, seps);
    }
  }
}

char* join(vector* v, const char* joiner)
{
  if (v->size == 0) {
    char * s = malloc(2);
    s[0] = '\0';
    return s;
  }
  size_t needed = 0;
  size_t pos = 0;
  size_t joiner_len = strlen(joiner);
  bool restore = false;
  if (v->data[v->size-1] == 0) {
    v->size--;
    restore = true;
  }
  for(size_t i = 0; i < v->size; i++) {
    needed += strlen(v->data[i]) + joiner_len;
  }

  char* s = malloc(needed + 3);

  for(size_t i = 0; i < v->size; i++) {
    size_t len = strlen(v->data[i]);
    memcpy(s + pos, v->data[i], len);
    pos += len;
    if (i != v->size-1) {
      memcpy(s + pos, joiner, joiner_len);
      pos += joiner_len;
    } else {
      pos += 1;
    }
  }
  s[pos-1] = '\0';
  if (restore)
    v->size++;

  return s;
}
