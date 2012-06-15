#ifndef CAPEL_VECTOR_H
#define CAPEL_VECTOR_H

#include <stdlib.h>
#include <functional.h>

typedef struct vector vector;

// Use this function to add an object to a vector.
void vector_push(vector* v, char* s);

vector* make_vector(int init_size);

void* vget(vector* v, size_t idx);
char* vcget(vector* v, size_t idx);
size_t vsize(vector* v);
void* vector_remove(vector*v, size_t i);
void cleanup_vector(vector* v);
char** vdata(vector* v);

// Splits a given string str into tokens based on sep, putting the tokens
// in a vector.
// Note: This function keeps an internal copy of the original string, so
// it should be passed to cleanup_vector with the FreeItems flag set when
// its lifespan has ended.
// The original string and seps are unmodified and can be safely freed.
vector* vector_split(const char * str, const char* seps);
vector* vector_split_quoted(const char * str, const char* seps);

void* vector_pop_front(vector* v);
char* vector_join(vector* v, const char* joiner);

#endif
