#include "hwloc_bitmap_wrapper.hpp"

hwloc_bitmap_wrapper hwloc_bitmap_make_wrapper() {
  return hwloc_bitmap_wrapper(hwloc_bitmap_alloc());
}

std::ostream& operator <<(std::ostream& s, const hwloc_bitmap_wrapper& w)
{
  char* tmp = nullptr;
  hwloc_bitmap_asprintf(&tmp, w.get());
  s << std::string(tmp);
  free(tmp);
  return s;
}
