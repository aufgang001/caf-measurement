#include "numa_test.hpp"
#include "generic_allocator.hpp"

#include <iostream>

using namespace std;

bool numa_test::init() {
  // topology should be freed even if an error occurs
  initialized = true;
  if (hwloc_topology_init(&topology_) == -1)
    return false; 
  if (hwloc_topology_load(topology_) == -1)
    return false; 
  topo_depth_ = hwloc_topology_get_depth(topology_);
  return true;
}

std::string numa_test::to_string(hwloc_obj_t& obj, int verbose) const {
  char str[128];
  hwloc_obj_type_snprintf(str, sizeof(str), obj, verbose);
  return string(str);
}

void numa_test::run_test() {
  if (!init()) {
    return; 
  }

  for (unsigned int depth = 0; depth < topo_depth_; ++depth) {
    cout << "*** Objects at level " << depth << endl;
    for (unsigned int i = 0; i < hwloc_get_nbobjs_by_depth(topology_, depth); ++i) {
      auto hwloc_obj = hwloc_get_obj_by_depth(topology_, depth, i);
      cout << "Index " << i << ": " << to_string(hwloc_obj) << endl;
    }
  }
}

numa_test::~numa_test() {
  if(initialized) 
    hwloc_topology_destroy(topology_);
}
