#include "numa_test.hpp"
#include "generic_allocator.hpp"

#include <iomanip>
#include <sstream>

using namespace std;


#define PRINT(fun, des) \
            cout << setw(width_a) << std::left << #fun << ": " \
            << setw(width_b) << fun() \
            << " (" << des << ")" << endl

bool numa_test::is_numa_available() const {
  return numa_available() != -1; 
}

std::ostream& operator<<(std::ostream& s, const bitmask* b) {
  if (b == nullptr) {
   return s << "nullptr";
  }
  ostringstream ss;
  for (auto x: numa_bitmask_wrapper(b)) {
    ss << x << " ";
  }
  if (ss.str().empty())
    s << "none";
  else
    s << ss.str();
  return s;
}

struct numa_pointer_handle {
  const bitmask* numa_all_nodes_ptr_fun() const {
    return numa_all_nodes_ptr;
  }
  const bitmask* numa_no_nodes_ptr_fun() const {
    return numa_no_nodes_ptr;
  }
  const bitmask* numa_all_cpus_ptr_fun() const {
    return numa_all_cpus_ptr;
  }
};

void numa_test::run_test() const {
  if (is_numa_available()) {
    print_stats();
    cout << endl;
    test_numa_parse_nodestring();
    cout << endl;
    test_numa_parse_cpustring();
    cout << endl;
    test_numa_set_preferred();
    cout << endl;
    test_numa_set_interleave_mask();
    cout << endl;
    test_memory_allocation();
    cout << endl;
    test_numa_run_on_node();
    cout << endl;
  }
}

void numa_test::numa_test_string_to_bitmask(const string& title,
                            function<bitmask*(const char*)> parse_fun) const {
  cout << title << endl;
  constexpr const char* tmp[] = {"1,2,3", "1-5",  "7,10, 11",
                                 "!4-6",  "+0-3", "all"};
  for(auto& x: tmp) {
    cout << "  " << std::left << setw(9) << x << ": " << parse_fun(x) << endl;
  
  }
}

void numa_test::test_numa_parse_nodestring() const {
  numa_test_string_to_bitmask("-- TEST numa_parse_nodestring() --",
                              numa_parse_nodestring);
}

void numa_test::test_numa_parse_cpustring() const {
  numa_test_string_to_bitmask("-- TEST numa_parse_cpustring() --",
                              numa_parse_cpustring);
}

void numa_test::test_numa_set_preferred() const {
  cout << "-- TEST numa_set_preferred() --" << endl;   
  auto print_current_node = []{
    cout << "  current node: " << numa_preferred() << endl; 
  };
  auto nodes = numa_bitmask_wrapper(numa_all_nodes_ptr);
  if (nodes.size() >= 2) {
    int first_node = *nodes.begin();
    int last_node = *(--nodes.end());
    cout << "  change to node " << first_node << endl;
    numa_set_preferred(first_node);
    print_current_node();
    cout << "  change to node " << last_node << endl;
    print_current_node();
    cout << "  reset to local node" << endl;
    numa_set_localalloc();
    print_current_node();
  } else {
    cout << "  to less NUMA nodes on this system" << endl;
  }
}

void numa_test::test_numa_set_interleave_mask() const {
  cout << "-- TEST numa_set_interleave_mask() --" << endl;   
  auto print_current_interleave_mask = []{
    cout << "  current_interleave_mask: " << numa_get_interleave_mask() << endl;
  };
  print_current_interleave_mask();
  cout << "  set to interlevae mask to numa_all_nodes_ptr" << endl;
  numa_set_interleave_mask(numa_all_nodes_ptr);
  print_current_interleave_mask();
  cout << "  set to interlevae mask to numa_no_nodes_ptr" << endl;
  numa_set_interleave_mask(numa_no_nodes_ptr);
  print_current_interleave_mask();
} 

void numa_test::test_memory_allocation() const {
  cout << "-- TEST test memory_allocation() --" << endl;   
  {
    auto alloc_fun = [] (size_t n) -> void* {
      return malloc(n); 
    };
    auto free_fun = [] (void* ptr, size_t) {
      free(ptr); 
    };
    vector<uint64_t, generic_allocator<uint64_t>> v(generic_allocator<uint64_t>(alloc_fun, free_fun));
    for (int i = 0; i < 100; ++i) {
      v.push_back(1);
    }
    cout << "  it works for malloc and free" << endl;
  }
  {
    auto alloc_fun = [] (size_t n) -> void* {
      return numa_alloc_local(n); 
    };
    auto free_fun = [] (void* ptr, size_t n) {
      numa_free(ptr, n);
    };
    vector<uint64_t, generic_allocator<uint64_t>> v(generic_allocator<uint64_t>(alloc_fun, free_fun));
    for (int i = 0; i < 100; ++i) {
      v.push_back(1);
    }
    cout << "  it works for numa_alloc_local and numa_free" << endl;
  }
}

void numa_test::test_numa_run_on_node() const {
  cout << "-- TEST test numa_run_on_node() --" << endl;   
  auto nodes = numa_bitmask_wrapper(numa_all_nodes_ptr);
  if (nodes.size() >= 2) {
    int first_node = *nodes.begin();
    int last_node = *(--nodes.end());
    cout << "  run current task on node " << first_node << endl;
    if (numa_run_on_node(first_node) == -1)
      cout << "  failed" << endl;
    cout << "  run current task on node " << last_node << endl;
    if (numa_run_on_node(last_node) == -1)
      cout << "  failed" << endl;
    cout << "  reset to default behavior" << endl;
    if (numa_run_on_node(-1) == -1)
      cout << "  failed" << endl;
  } else {
    cout << "  to less NUMA nodes on this system" << endl;
  }
}

void numa_test::print_stats() const {
  int width_a = 27;
  int width_b = 4;
  cout << "-- NUMA Statistics --" << endl;
  numa_pointer_handle nph; 
  PRINT(numa_available, "if -1, all other NUMA functions are undefined");
  PRINT(numa_max_possible_node, "highest possible node on your system");
  PRINT(numa_num_possible_nodes, "maximum number of nodes this kernel can handle");
  PRINT(numa_max_node, "highest node number available on this system");
  PRINT(numa_num_configured_nodes, "number of memory nodes on this system");
  PRINT(numa_get_mems_allowed, "this process is allowed to allocate memory from these nodes");
  PRINT(numa_num_configured_cpus, "number of cpus in the system");
  PRINT(nph.numa_all_nodes_ptr_fun, "all nodes from which this process may allocate memory");
  PRINT(nph.numa_no_nodes_ptr_fun, "set with no nodes");
  PRINT(nph.numa_all_cpus_ptr_fun, "all cpus on which this task may execute");
  PRINT(numa_num_task_cpus, "number of cpus this task is allowed to use");
  PRINT(numa_num_task_nodes, "number of nodes this task is allowed to allocate memory");
  cout << setw(width_a) << "numa_node_size" << ": (Memory size of NUMA nodes (total/free)): " << endl;
  for (auto node: numa_bitmask_wrapper(numa_all_nodes_ptr)) {
    long total_size = numa_node_size(node, nullptr);
    long tmp = 1;
    long free_size = numa_node_size(node, &tmp);
    if (total_size != -1) {
      cout << "  Node " << node << ": " << total_size / 1024 / 1024 / 1024
           << "GB / " << free_size / 1024 / 1024 / 1024 << "GB ("
           << total_size << " / " << free_size << ")" << endl;
    } else {
      cout << "  Node " << node << ": ERROR" << endl;
    }
  }
  PRINT(numa_preferred, "preferred (memory) node of the current task");
  PRINT(numa_get_interleave_mask, "interleave mask if policy is page interleave");
  PRINT(numa_get_membind, "nodes from which memory can currently be allocated");
  PRINT(numa_get_run_node_mask, "nodes on which the current task is allowed to run");
}

//TODO
//memory leak??? deallocate bitmasks (allocate_nodemask, allocate_cpumask)
