#ifndef CAPEL_VECTOR_H
#define CAPEL_VECTOR_H

#include <stdlib.h>
#include <functional.h>

#define fori(x, n) for(size_t i = 0; i < n; i++)

// An auto-expanding array. You must use make_vector(data_type_size) to make it 
// and vector_push to add items to it. When you are finished, call cleanup_vector.
// data_type_size is the size of the data type you are storing in the array.
// example: vector* v = make_vector(sizeof(whatever*));
typedef struct
{
  // Public data -- use these and the provided vector functions.
  char** data;
  size_t size;

  // Private data -- do not use them.
  size_t __allocated_size;
  // This is used for the split_to_vector function.
  char* __source;
} vector;

// Use this function to add an object to a vector.
void vector_push(vector* v, char* s);

vector* make_vector(int init_size);

// Used to remove an item from a vector. It removes the item at i and returns 
// it if cleanup_type is UNMANAGED_POINTERS; if it is anything else, it frees it
// and returns 0. Items are shifted left.
void* vector_remove(vector*v, size_t i);

// Call this to cleanup the vector.
// Its actions depend on what type of cleanup the vector was created with.
void cleanup_vector(vector* v);

// Splits a given string str into tokens based on sep, putting the tokens
// in a vector.
// Note: This function keeps an internal copy of the original string, so
// it should be passed to cleanup_vector with the FreeItems flag set when
// its lifespan has ended.
// The original string and seps are unmodified and can be safely freed.
vector* vector_split(const char * str, const char* seps);

// Prints out a vector
// format_string will be passed once to printf() for each argument
// therefore, make sure it only has one variable in the format and
// that it is prepared for whatever datatype is in the vector. 
void print_vector(vector* v, const char* format_string, size_t start);

void* vector_pop_front(vector* v);

const char* vector_join(vector* v, const char* joiner);
#endif
