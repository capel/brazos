#ifndef CAPEL_VECTOR_H
#define CAPEL_VECTOR_H

#include "malloc.h"


// Pass one of the to make_vector.
// If you want to manually manage memory, use UNMANAGED_POINTERS.
// If you want it to free its items when it is cleaned up, use MANAGED_POINTERS.
enum cleanup_type
{
  MANAGED_POINTERS, // It is a vector of pointers that will be freed by the vector on cleanup.
  UNMANAGED_POINTERS, // It is a vector of pointers that the vector does not attempt to free on cleanup.
  __SPLIT_TO_VECTOR // A vector produced by split_to_vector. Internal use only: do not use directly.
};


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
        enum cleanup_type __type;
        size_t __allocated_size;
        size_t __data_type_size;
		// This is used for the split_to_vector function.
		char* __source;
} vector;

// Use this function to add an object to a vector.
void vector_push(vector* v, char* s);

// Use this function to create a vector. data_type_size should be
// the sizeof() what you want to store in the vector.
// example: vector* v = make_vector(sizeof(whatever*));
vector* kmake_vector(int data_type_size, enum cleanup_type type);

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
vector* ksplit_to_vector(const char * str, const char* seps);

// Prints out a vector
// format_string will be passed once to printf() for each argument
// therefore, make sure it only has one variable in the format and
// that it is prepared for whatever datatype is in the vector. 
void print_vector(vector* v, const char* format_string, size_t start);


const char* vector_join(vector* v, const char* joiner);
#endif
