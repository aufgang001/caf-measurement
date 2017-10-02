#ifndef HWLOC_CPP_HELPER_HPP
#define HWLOC_CPP_HELPER_HPP

#include <hwloc.h>
#include <memory>
#include <ostream>

// topology wrapper
struct hwloc_topo_free {
  void operator()(hwloc_topology_t p) {
    hwloc_topology_destroy(p);
  }
};

using topo_ptr = std::unique_ptr<hwloc_topology, hwloc_topo_free>;

topo_ptr hwloc_make_topology_wrapper();

// bitmap wrapper
struct hwloc_bitmap_free_wrapper {
  void operator()(hwloc_bitmap_t p) {
    hwloc_bitmap_free(p);
  }
};

using bitmap_wrapper_t =
  std::unique_ptr<hwloc_bitmap_s, hwloc_bitmap_free_wrapper>;

bitmap_wrapper_t hwloc_bitmap_make_wrapper();

// allocator
struct hwloc_mem_deposer {
  hwloc_mem_deposer(hwloc_topology_t topo, size_t size)
    : topo(topo)
    , size(size) {
    // nop 
  }

  void operator()(void * p) {
    hwloc_free(topo, p, size);
  }

  hwloc_topology_t topo;
  size_t size;
};


// plot helper
std::ostream& operator<<(std::ostream& s, const bitmap_wrapper_t& w);

#endif // HWLOC_CPP_HELPER_HPP
