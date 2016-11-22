#include "numa_test.hpp"
#include "generic_allocator.hpp"
#include "numa_topo.hpp"

#include <limits>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <sstream>

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


std::string numa_test::bitmap_to_string(const hwloc_bitmap_wrapper& bmap) const {
  std::stringstream ss; 
  unsigned int id;
  // Check if bitmap has infinit length
  if (hwloc_bitmap_weight(bmap.get()) != -1) {
    hwloc_bitmap_foreach_begin(id, bmap.get()) {
      ss << id << " ";
    }
    hwloc_bitmap_foreach_end();
  } else {
    ss << "infinite"; 
  }
  return ss.str();
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

void numa_test::print_distance(const hwloc_distances_s* dm) const {
  cout << "index ";
  for (unsigned int i = 0; i < dm->nbobjs; ++i) {
    cout << setw(5) << i << " ";
  }
  cout << endl;
  for (unsigned int i = 0; i < dm->nbobjs; ++i) {
    cout << setw(5) << i << " ";
    for (unsigned int j = 0; j < dm->nbobjs; ++j) {
      cout << setw(5) << dm->latency[i * dm->nbobjs + j] << " ";
    }
    cout << endl;
  }
}

hwloc_bitmap_wrapper numa_test::get_current_cpu_bind() const {
  auto current_cpuset = hwloc_bitmap_make_wrapper();
  hwloc_get_cpubind(topology_, current_cpuset.get(),
                        HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND);
  return current_cpuset;
}

hwloc_bitmap_wrapper numa_test::get_last_cpu_location() const {
  auto current_cpuset= hwloc_bitmap_make_wrapper();
  hwloc_get_last_cpu_location(topology_, current_cpuset.get(),
                              HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND);
  return current_cpuset;
}

tuple<hwloc_bitmap_wrapper, hwloc_membind_policy_t>
numa_test::get_mem_bind() const {
  auto current_cpuset = hwloc_bitmap_make_wrapper();
  hwloc_membind_policy_t mem_policy;
  hwloc_get_membind(topology_, current_cpuset.get(), &mem_policy,
                    HWLOC_MEMBIND_THREAD);
  return make_tuple(std::move(current_cpuset), mem_policy);
}

std::tuple<hwloc_bitmap_wrapper, hwloc_membind_policy_t>
numa_test::get_mem_bind_nodeset() const {
  auto current_cpuset = hwloc_bitmap_make_wrapper();
  hwloc_membind_policy_t mem_policy;
  hwloc_get_membind_nodeset(topology_, current_cpuset.get(), &mem_policy,
                            HWLOC_MEMBIND_THREAD);
  return make_tuple(std::move(current_cpuset), mem_policy);
}

void numa_test::bind_current_thread() const{
  auto new_cpuset = hwloc_bitmap_make_wrapper();
  hwloc_bitmap_set(new_cpuset.get(), 1);
  cout << "bind thread to cpu:" << new_cpuset << endl;
  if (hwloc_set_cpubind(topology_, new_cpuset.get(),
                        HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND) != 0) {
    cerr <<  "hwloc_set_cpubind failed" << endl;
  }

  auto new_nodeset = hwloc_bitmap_make_wrapper();
  hwloc_cpuset_to_nodeset (topology_, new_cpuset.get(), new_nodeset.get());
  cout << "bind memory allocation to numa node:" << new_nodeset << endl;
  if (hwloc_set_membind_nodeset(topology_, new_nodeset.get(),
                                HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_THREAD)
      != 0) {
    cerr <<  "hwloc_set_membind failed" << endl;
  }
}

void numa_test::test_distance(){
  cout << "##-- test_distance --##" << endl;
  const hwloc_distances_s* dm =
    hwloc_get_whole_distance_matrix_by_type(topology_, HWLOC_OBJ_NUMANODE);
  if (!dm || !dm->latency) {
    cout << "hwloc_get_whole_distance_matrix_by_type failed" << endl;
    return;
  }
  cout << "  relative_depth: " << dm->relative_depth << endl;
  cout << "  nbobjs: " << dm->nbobjs << endl;
  cout << "  latency_max: " << dm->nbobjs << endl;
  cout << "  latency_base: " << dm->nbobjs << endl;
  cout << "  latencies: " << endl;
  print_distance(dm);

}

void numa_test::test_pin_thread() {
  cout << "##-- get current cpu bind --##" << endl;
  cout << "current cpu binding: " << get_current_cpu_bind() << endl;
  cout << "last cpu location: " << get_last_cpu_location() << endl;
  auto mem_bind_nodeset = get_mem_bind_nodeset();
  cout << "mem_bind_nodeset: " << get<0>(mem_bind_nodeset)
       << " with policy:" << membind_policy_to_string(get<1>(mem_bind_nodeset))
       << endl;
  bind_current_thread();
  cout << "current cpu binding: " << get_current_cpu_bind() << endl;
  cout << "last cpu location: " << get_last_cpu_location() << endl;
  mem_bind_nodeset = get_mem_bind_nodeset();
  cout << "mem_bind_nodeset: " << get<0>(mem_bind_nodeset)
       << " with policy:" << membind_policy_to_string(get<1>(mem_bind_nodeset))
       << endl;
}

void numa_test::test_allocate_memory() {
  cout << "##-- test_allocate_memory --##" << endl;
  auto new_cpuset = hwloc_bitmap_make_wrapper();
  auto new_nodeset = hwloc_bitmap_make_wrapper();
  hwloc_bitmap_set(new_cpuset.get(), 1);
  hwloc_cpuset_to_nodeset (topology_, new_cpuset.get(), new_nodeset.get());
  cout << "alloc node_set: " << new_nodeset << endl;
  auto alloc = generic_allocator<int>(
    [&](size_t n) -> void* {
      cout << "alloc: " << n << endl;
      return hwloc_alloc_membind_nodeset(topology_, n, new_nodeset.get(),
                                         HWLOC_MEMBIND_BIND,
                                         HWLOC_MEMBIND_THREAD);
    },
    [&](void* ptr, size_t n) {
      cout << "free: " << n << endl;
      hwloc_free(topology_, ptr, n);
    });
  vector<int, generic_allocator<int>> v(alloc);

  for (int i = 0; i < 10000; ++i) {
    v.push_back(i);
  }
}

void numa_test::test_create_scheduling_hierarchy(int cpu_id) {
  cout << "##-- test_create_scheduling_hierarchy --##" << endl;
  numa_topo s(topology_);
  auto current_pu_set = hwloc_bitmap_make_wrapper();
  auto node_set = hwloc_bitmap_make_wrapper();
  hwloc_bitmap_set(current_pu_set.get(), cpu_id); 
  auto pu_matrix = s.get_pu_matrix(current_pu_set);
  hwloc_cpuset_to_nodeset(topology_, node_set.get(), node_set.get());
  cout << "current pu id: " << current_pu_set 
       << " nodeset(" << bitmap_to_string(node_set) << ")" 
       << endl;
  for(unsigned int i = 0; i < pu_matrix.size(); ++i) {
    hwloc_cpuset_to_nodeset(topology_, pu_matrix[i].get(), node_set.get());
    cout << i << ": " << pu_matrix[i] 
         << " nodeset(" << bitmap_to_string(node_set) << ")" 
         << endl;
  }
}



void numa_test::run_test() {
  if (!init()) {
    return; 
  }
  print_levels_and_objects();
  print_topo_tree(); 

  test_pin_thread();
  test_allocate_memory();
  test_distance();
  test_create_scheduling_hierarchy(2);
  test_create_scheduling_hierarchy(12);
  test_create_scheduling_hierarchy(59);
}

numa_test::~numa_test() {
  if(initialized) 
    hwloc_topology_destroy(topology_);
}
