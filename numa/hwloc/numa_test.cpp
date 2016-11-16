#include "numa_test.hpp"
#include "generic_allocator.hpp"
#include "hwloc_bitmap_wrapper.hpp"

#include <limits>
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

// API Examaple
//http://www.open-mpi.de/projects/hwloc/doc/v1.11.4/

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

std::string numa_test::obj_to_string(const hwloc_obj_t obj, int verbose) const {
  char str[128];
  hwloc_obj_type_snprintf(str, sizeof(str), obj, verbose);
  return string(str);
}

std::string numa_test::attr_to_string(const hwloc_obj_t obj,
                                      const char* seperator,
                                      int verbose) const {
  char str[1024];
  hwloc_obj_attr_snprintf(str, sizeof(str), obj, seperator, verbose);
  return string(str); 
}

std::string
numa_test::membind_policy_to_string(hwloc_membind_policy_t policy) const {
  std::map<hwloc_membind_policy_t, string> policy_map = {
    {HWLOC_MEMBIND_DEFAULT,	   "HWLOC_MEMBIND_DEFAULT"},
    {HWLOC_MEMBIND_FIRSTTOUCH, "HWLOC_MEMBIND_FIRSTTOUCH"},
    {HWLOC_MEMBIND_BIND,       "HWLOC_MEMBIND_BIND"},
    {HWLOC_MEMBIND_INTERLEAVE, "HWLOC_MEMBIND_INTERLEAVE"},
    {HWLOC_MEMBIND_REPLICATE,  "HWLOC_MEMBIND_REPLICATE"},
    {HWLOC_MEMBIND_NEXTTOUCH,  "HWLOC_MEMBIND_NEXTTOUCH"},
    {HWLOC_MEMBIND_MIXED,      "HWLOC_MEMBIND_MIXED"}
  };
  return policy_map[policy];
}

void numa_test::print_children(const hwloc_obj_t obj, unsigned int depth) const {
  auto str_obj = obj_to_string(obj);
  cout << setw(depth + str_obj.size()) << str_obj; 
  if (obj->os_index != numeric_limits<unsigned int>().max()){
    cout << "#" << obj->os_index; 
  }
  string attr = attr_to_string(obj);
  if (!attr.empty()) 
    cout <<"(" << attr << ")";
  cout << endl;
  // iterate over all children
  for (unsigned int i = 0; i < obj->arity; ++i) {
    print_children(obj->children[i], depth + 1);
  }
}

void numa_test::print_levels_and_objects() const {
  cout << "##-- Topo Levels and Objects --##" << endl;
  for (unsigned int depth = 0; depth < topo_depth_; ++depth) {
    auto num_of_objects = hwloc_get_nbobjs_by_depth(topology_, depth);
    auto hwloc_obj = hwloc_get_obj_by_depth(topology_, depth, 0);
    cout << "Depth " << depth << ": " << num_of_objects << "x " << obj_to_string(hwloc_obj, 0) << endl;
  }
}

void numa_test::print_topo_tree() const {
  cout << "##-- Topo Tree --##" << endl;
  print_children(hwloc_get_root_obj(topology_));
}

hwloc_bitmap_wrapper numa_test::get_current_cpu_bind() const {
  hwloc_bitmap_wrapper current_cpuset;
  hwloc_get_cpubind(topology_, current_cpuset.get(),
                        HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND);
  return current_cpuset;
}

hwloc_bitmap_wrapper numa_test::get_last_cpu_location() const {
  hwloc_bitmap_wrapper current_cpuset;
  hwloc_get_last_cpu_location(topology_, current_cpuset.cget(), HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND);
  return current_cpuset;
}

tuple<hwloc_bitmap_wrapper,hwloc_membind_policy_t> numa_test::get_mem_bind() const {
  hwloc_bitmap_wrapper current_cpuset;
  hwloc_membind_policy_t mem_policy;
  hwloc_get_membind(topology_, current_cpuset.get(), &mem_policy, HWLOC_MEMBIND_THREAD);
  return make_tuple(current_cpuset, mem_policy);
}

std::tuple<hwloc_bitmap_wrapper,hwloc_membind_policy_t> numa_test::get_mem_bind_nodeset() const {
  hwloc_bitmap_wrapper current_cpuset;
  hwloc_membind_policy_t mem_policy;
  hwloc_get_membind_nodeset(topology_, current_cpuset.get(), &mem_policy, HWLOC_MEMBIND_THREAD);
  return make_tuple(std::move(current_cpuset), mem_policy);
}

void numa_test::bind_current_thread() const{
  hwloc_bitmap_wrapper new_cpuset;
  new_cpuset.set(1);
  cout << "bind thread to cpu:" << new_cpuset << endl;
  if (hwloc_set_cpubind(topology_, new_cpuset.cget(),
                        HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND) != 0) {
    cerr <<  "hwloc_set_cpubind failed" << endl;
  }

  hwloc_bitmap_wrapper new_nodeset;
  hwloc_cpuset_to_nodeset (topology_, new_cpuset.get(), new_nodeset.get());
  cout << "### test node_set: " << new_nodeset << endl;
  cout << "bind memory allocation to numa node:" << new_cpuset << endl;
  if (hwloc_set_membind_nodeset(topology_, new_nodeset.get(),
                                HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_THREAD)
      != 0) {
    cerr <<  "hwloc_set_membind failed" << endl;
  }
}

void numa_test::test_pin_thread() {
  cout << "##-- get current cpu bind --##" << endl;
  cout << "current cpu binding: " << get_current_cpu_bind() << endl;
  cout << "last cpu location: " << get_last_cpu_location() << endl;
  auto mem_bind = get_mem_bind();
  auto mem_bind_nodeset = get_mem_bind_nodeset();
  cout << "mem_bind: " << get<0>(mem_bind) << " with policy:" << membind_policy_to_string(get<1>(mem_bind)) << endl;
  cout << "mem_bind_nodeset: " << get<0>(mem_bind_nodeset) << " with policy:" << membind_policy_to_string(get<1>(mem_bind_nodeset)) << endl;
  bind_current_thread();
  cout << "current cpu binding: " << get_current_cpu_bind() << endl;
  cout << "last cpu location: " << get_last_cpu_location() << endl;
  mem_bind = get_mem_bind();
  mem_bind_nodeset = get_mem_bind_nodeset();
  cout << "mem_bind: " << get<0>(mem_bind) << " with policy:" << membind_policy_to_string(get<1>(mem_bind)) << endl;
  cout << "mem_bind_nodeset: " << get<0>(mem_bind_nodeset) << " with policy:" << membind_policy_to_string(get<1>(mem_bind_nodeset)) << endl;
}

void numa_test::test_allocate_memory() {
}

void numa_test::test_create_scheduling_hierarchy() {
}

void numa_test::run_test() {
  if (!init()) {
    return; 
  }
  print_levels_and_objects();
  print_topo_tree(); 

  test_pin_thread();
  test_allocate_memory();
  test_create_scheduling_hierarchy();
}

numa_test::~numa_test() {
  if(initialized) 
    hwloc_topology_destroy(topology_);
}
