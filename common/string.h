#ifndef COMMON_STRING_H
#define COMMON_STRING_H

#include "ptr.h"
#include "stdlib.h"

// An IMMUTABLE string class. Note the immutable.
class string {
  public:
    //string(const char* s) : m_size(strlen(s)+1), m_s(str_clone(s, m_size)) {}
    string(const string &s) : m_s(s.m_s), m_size(s.m_size) {}
    const char operator[](size_t idx) { 
      if (idx > m_size) return 0;
      return m_s.get()[idx];
    }
    size_t size() const { return m_size; }
    char* clone() const { return str_clone(m_s.get()); }
    const char* c_str() const { return m_s.get() }
    bool operator==(const string &s) const { return *this == s.c_str(); }
    bool operator==(const char* s) const { return 0 == strcmp(m_s.get(), s); }
    bool operator!=(const string &s) const { return !(*this == s); }

    const string& operator=(const string &s) {
      m_size = s.m_size;
      m_s = s.m_s;
      return *this;
    }

    //shared_ptr<vector<const char*> > split(const char* seps);
  private:
    const size_t m_size;
    shared_ptr<const char*> m_s;

    static const char* str_clone(const char* s, size_t len) {
      char* n = new char[len];
      strcpy(n, s, len);
      return n;
    }
};


#endif
