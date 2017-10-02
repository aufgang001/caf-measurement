#include "hwloc_cpp_helper.hpp"

topo_ptr hwloc_make_topology_wrapper() {
  topo_ptr result;
  hwloc_topology_t raw_topo;
  auto err = hwloc_topology_init(&raw_topo);
  if (err == -1)
    throw "hwloc_topology_init() failed";
  result.reset(raw_topo);
  err = hwloc_topology_load(result.get());
  if (err == -1)
    throw "hwloc_topology_load() failed";
  return result;
}

bitmap_wrapper_t hwloc_bitmap_make_wrapper() {
  return bitmap_wrapper_t(hwloc_bitmap_alloc());
}

std::ostream& operator<<(std::ostream& s, const bitmap_wrapper_t& w) {
  char* tmp = nullptr;
  hwloc_bitmap_asprintf(&tmp, w.get());
  s << std::string(tmp);
  free(tmp);
  return s;
}
