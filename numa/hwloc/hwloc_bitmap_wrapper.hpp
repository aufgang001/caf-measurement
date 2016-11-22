#ifndef HWLOC_BITMAP_WRAPPER_HPP
#define HWLOC_BITMAP_WRAPPER_HPP

#include <hwloc.h>
#include <memory>
#include <string>

#include <cstdlib>

struct hwloc_bitmap_wrapper_free {
  void operator()(hwloc_bitmap_t p) {
    hwloc_bitmap_free (p);
  }
};

using hwloc_bitmap_wrapper =
  std::unique_ptr<hwloc_bitmap_s, hwloc_bitmap_wrapper_free>;

hwloc_bitmap_wrapper hwloc_bitmap_make_wrapper(); 
std::ostream& operator <<(std::ostream& s, const hwloc_bitmap_wrapper& w);

#endif // HWLOC_BITMAP_WRAPPER_HPP
