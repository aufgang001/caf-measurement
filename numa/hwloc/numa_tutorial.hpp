#ifndef NUMA_TUTORIAL_HPP
#define NUMA_TUTORIAL_HPP

#include <hwloc.h>
#include <errno.h>
#include <string>

class numa_tutorial {
public:
  bool init();
  ~numa_tutorial();
  std::string obj_to_string(const hwloc_obj_t obj, int verbose = 0) const;
  std::string attr_to_string(const hwloc_obj_t obj,
                             const char* seperator = " ",
                             int verbose = 0) const;
  std::string bitmap_to_string(const hwloc_bitmap_t bitmap) const;
  void print_children(const hwloc_obj_t obj, unsigned int depth = 0) const;
  void print_levels_and_objects() const;
  void print_topo_tree() const;

  void test_get_number_of_packages() const;
  void test_sum_cache_size_first_processor_has_above_it() const;
  void test_bind_to_only_on_thread_of_the_last_core_of_the_machine() const;
  void test_allcoate_memory_on_numa_node() const;


  void run_test();
private:
  bool initialized; 
  unsigned int topo_depth_;
  hwloc_topology_t topology_;

};

#endif // NUMA_TUTORIAL_HPP
