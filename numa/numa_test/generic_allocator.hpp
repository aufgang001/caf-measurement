#ifndef GENERIC_ALLOCATOR_HPP
#define GENERIC_ALLOCATOR_HPP

#include <cstddef>
#include <functional>

template <class T>
class generic_allocator {
public:
  using value_type = T;
  using alloc_fun = std::function<void*(size_t)>;
  using free_fun = std::function<void(void*, size_t)>;

  generic_allocator(const alloc_fun& a, const free_fun& f)
      : alloc_(a)
      , free_(f) {
  } 
  
  template <class U> 
  generic_allocator(const generic_allocator<U>&) {}
  
  value_type* allocate(std::size_t n) {
    return static_cast<value_type*>(alloc_(sizeof(value_type) * n));
  }

  void deallocate(value_type* p, size_t n) {
    free_(p, sizeof(value_type) * n);
  }
private: 
  alloc_fun alloc_;
  free_fun free_;
};

template <class T, class U>
bool operator==(const generic_allocator<T>&, const generic_allocator<U>&) {
  return false;
}
template <class T, class U>
bool operator!=(const generic_allocator<T>& a, const generic_allocator<U>& b) {
  return !(a == b); 
}

#endif // GENERIC_ALLOCATOR_HPP
