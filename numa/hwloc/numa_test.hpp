#ifndef NUMA_TEST_HPP
#define NUMA_TEST_HPP

#include <errno.h>
#include <functional>
#include <hwloc.h>
#include <string>

#include "hwloc_bitmap_wrapper.hpp"

class numa_test {
public:
  bool init();
  ~numa_test();
  std::string obj_to_string(const hwloc_obj_t obj, int verbose = 0) const;
  std::string attr_to_string(const hwloc_obj_t obj, const char* seperator = " ",
                             int verbose = 0) const;
  std::string membind_policy_to_string(hwloc_membind_policy_t policy) const;
  void print_children(const hwloc_obj_t obj, unsigned int depth = 0) const;
  void print_distance(const hwloc_distances_s* distance) const;
  void print_levels_and_objects() const;
  void print_topo_tree() const;

  // cpu binding
  hwloc_bitmap_wrapper get_current_cpu_bind() const;
  hwloc_bitmap_wrapper get_last_cpu_location() const;
  std::tuple<hwloc_bitmap_wrapper, hwloc_membind_policy_t> get_mem_bind() const;
  std::tuple<hwloc_bitmap_wrapper, hwloc_membind_policy_t>
  get_mem_bind_nodeset() const;

  void bind_current_thread() const; // task and memory

  void test_pin_thread();
  void test_allocate_memory();
  void test_distance();
  void test_create_scheduling_hierarchy();

  void run_test();

private:
  bool initialized;
  unsigned int topo_depth_;
  hwloc_topology_t topology_;
};

#endif // NUMA_TEST_HPP
