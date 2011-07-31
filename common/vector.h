#ifndef VECTOR_H
#define VECTOR_H

#include "ptr.h"

class string;

template<typename T>
class vector : public noncopyable
{
  public:
    vector(size_t size);
    virtual ~vector();
    T operator[](size_t idx) const { return m_data[idx]; }
    size_t size() const { return m_size; }
    void push(T t);
    T pop();
    void remove(size_t idx);
    bool empty() { return size() == 0 };
  private: 
    T* m_data;
    size_t m_allocated;
    size_t m_size;
};

/*
class SplitStringVector : public vector<string> {
  public:
    SplitStringVector(string s, string seps);
    ~SplitStringVector()
  private:
    char * m_source;
};
*/

//typedef bool (*vector_better_func)(void* a, void* b);
// Finds the best element (as given by the better function) in the vector
// and returns it.
//void* vector_best(vector* v, vector_better_func better);
#endif
