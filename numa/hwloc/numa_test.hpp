#ifndef NUMA_TEST_HPP
#define NUMA_TEST_HPP

#include <hwloc.h>
#include <errno.h>
#include <string>

class numa_test {
public:
  bool init();
  ~numa_test(); 
  std::string to_string(hwloc_obj_t& obj, int verbose = 0) const;
  void run_test();
private:
  bool initialized; 
  unsigned int topo_depth_;
  hwloc_topology_t topology_;

};

#endif // NUMA_TEST_HPP
