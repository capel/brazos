#ifndef COMMON_SHARED_PTR_H
#define COMMON_SHARED_PTR_H

template<typename K, typename V>
class pair {
  public:
    pair(K k, V v) : m_car(k), m_cdr(v) {}
    K car() const { return m_car; }
    V cdr() const { return m_cdr; }

  private:
    K m_car;
    V m_cdr;
};

class noncopyable {
  private:
    noncopyable(const noncopyable& nc);
    const noncopyable& operator=(const noncopyable& nc);
};

template<typename T>
class scoped_ptr : public noncopyable {
  private:
    T* p;
  public:
    typedef T * scoped_ptr<T>::*boolish;
    explicit scoped_ptr(T* p = 0) : p(p) {}

    T& operator*() const { return *p; }
    T* operator->() const { return p; }
    T* get() const { return p; }
    operator boolish() const { return p == 0 ? 0 : &scoped_ptr<T>::p; }
    ~T() {
      delete p;
    }
};

template<typename T>
class shared_ptr {
  private:
    int *rc;
    T* p;
    void cleanup() {
      delete p;
      delete rc;
    }
  typedef T * shared_ptr<T>::*boolish;
  public:
    explicit shared_ptr(T* p = 0) : p(p), rc(new int) { *rc = 1; }
    explicit shared_ptr(const shared_ptr<T>& sp) : rc(sp.rc), p(sp.p) {
      (*rc)++;
    }

    shared_ptr<T>& operator = (const shared_ptr<T>& sp) {
      if (this == &sp) return *this;
      (*rc)--;
      if (!*rc) {
        cleanup();
      }

      rc = sp.rc;
      p = sp.p;
      (*sp)++;
    }
      
    ~shared_ptr() { 
      (*rc)--;
      if (!*rc) cleanup();
    }
    T& operator*() const { return *p; }
    T* operator->() const { return p; } 
    T* get() const { return p; }
    operator boolish() const { return p == 0 ? 0 : &shared_ptr<T>::p; }
};


#endif
