#include "numa_tutorial.hpp"
#include "generic_allocator.hpp"

#include <limits>
#include <iostream>
#include <iomanip>

using namespace std;

// API Examaple
//http://www.open-mpi.de/projects/hwloc/doc/v1.11.4/

bool numa_tutorial::init() {
  // topology should be freed even if an error occurs
  initialized = true;
  if (hwloc_topology_init(&topology_) == -1)
    return false; 
  if (hwloc_topology_load(topology_) == -1)
    return false; 
  topo_depth_ = hwloc_topology_get_depth(topology_);
  return true;
}

std::string numa_tutorial::obj_to_string(const hwloc_obj_t obj, int verbose) const {
  char str[128];
  hwloc_obj_type_snprintf(str, sizeof(str), obj, verbose);
  return string(str);
}

std::string numa_tutorial::attr_to_string(const hwloc_obj_t obj, const char* seperator, int verbose) const {
  char str[1024];
  hwloc_obj_attr_snprintf(str, sizeof(str), obj, seperator, verbose);
  return string(str); 
}

std::string numa_tutorial::bitmap_to_string(const hwloc_bitmap_t bitmap) const {
  char *str;
  hwloc_bitmap_asprintf(&str, bitmap);
  string result(str);
  free(str);
  return result;
}

void numa_tutorial::print_children(const hwloc_obj_t obj, unsigned int depth) const {
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

void numa_tutorial::test_sum_cache_size_first_processor_has_above_it() const {
  cout << "##-- Compute the amount of cache that the first logical processor "
          "has above it --##"
       << endl;
    /*****************************************************************
     * Fourth example:
     * Compute the amount of cache that the first logical processor
     * has above it.
     *****************************************************************/
  int levels = 0;
  int size = 0;
  for (hwloc_obj_t obj = hwloc_get_obj_by_type(topology_, HWLOC_OBJ_PU, 0); obj;
       obj = obj->parent)
    if (obj->type == HWLOC_OBJ_CACHE) {
      levels++;
      size += obj->attr->cache.size;
    }
  cout << "*** Logical processor 0 has " << levels << " caches totaling " << size /1024 << " KB" << endl;
}
void numa_tutorial::test_bind_to_only_on_thread_of_the_last_core_of_the_machine() const {
  cout << "##--Bind to only one thread of the last core of the machine. --##"
       << endl;
  /*****************************************************************
   * Fifth example:
   * Bind to only one thread of the last core of the machine.
   *
   * First find out where cores are, or else smaller sets of CPUs if
   * the OS doesn't have the notion of a "core".
   *****************************************************************/
  int depth = hwloc_get_type_or_below_depth(topology_, HWLOC_OBJ_CORE);
  /* Get last core. */
  auto obj = hwloc_get_obj_by_depth(
    topology_, depth, hwloc_get_nbobjs_by_depth(topology_, depth) - 1);
  if (obj) {
    /* Get a copy of its cpuset that we may modify. */
    auto cpuset = hwloc_bitmap_dup(obj->cpuset);
    /* Get only one logical processor (in case the core is
       SMT/hyper-threaded). */
    hwloc_bitmap_singlify(cpuset);
    /* And try to bind ourself there. */
    if (hwloc_set_cpubind(topology_, cpuset, 0)) {
      int error = errno;
      cout << "Couldn't bind t cpuset " << bitmap_to_string(obj->cpuset) << ":"
           << strerror(error) << endl;
    }
    /* Free our cpuset copy */
    hwloc_bitmap_free(cpuset);
  }
}


void numa_tutorial::test_allcoate_memory_on_numa_node() const {
    /*****************************************************************
     * Sixth example:
     * Allocate some memory on the last NUMA node, bind some existing
     * memory to the last NUMA node.
     *****************************************************************/
    /* Get last node. */
    int n = hwloc_get_nbobjs_by_type(topology_, HWLOC_OBJ_NUMANODE);
    if (n) {
        void *m;
        int size = 1024*1024; //Kbyte
        auto obj = hwloc_get_obj_by_type(topology_, HWLOC_OBJ_NUMANODE, n - 1);
        m = hwloc_alloc_membind_nodeset(topology_, size, obj->nodeset,
                HWLOC_MEMBIND_BIND, 0);
        hwloc_free(topology_, m, size);
        m = malloc(size);
        hwloc_set_area_membind_nodeset(topology_, m, size, obj->nodeset,
                HWLOC_MEMBIND_BIND, 0);
        free(m);
    }
}

void numa_tutorial::print_levels_and_objects() const {
  cout << "##-- Topo Levels and Objects --##" << endl;
  for (unsigned int depth = 0; depth < topo_depth_; ++depth) {
    auto num_of_objects = hwloc_get_nbobjs_by_depth(topology_, depth);
    auto hwloc_obj = hwloc_get_obj_by_depth(topology_, depth, 0);
    cout << "Depth " << depth << ": " << num_of_objects << "x " << obj_to_string(hwloc_obj, 0) << endl;
  }
}

void numa_tutorial::test_get_number_of_packages() const {
  cout << "##-- Number of Packages --##" << endl;
  auto depth = hwloc_get_type_depth(topology_, HWLOC_OBJ_PACKAGE);
  if (depth == HWLOC_TYPE_DEPTH_UNKNOWN)
    cout << "Number of packages is unknown" << endl;
  else
    cout << hwloc_get_nbobjs_by_depth(topology_, depth) << " packages(s)" << endl;
}

void numa_tutorial::print_topo_tree() const {
  printf("*** Printing overall tree\n");
  cout << "##-- Topo Tree --##" << endl;
  print_children(hwloc_get_root_obj(topology_));
}

void numa_tutorial::run_test() {
  if (!init()) {
    return; 
  }
  print_levels_and_objects();
  print_topo_tree(); 
  test_get_number_of_packages();
  test_sum_cache_size_first_processor_has_above_it();
  test_bind_to_only_on_thread_of_the_last_core_of_the_machine();
  test_allcoate_memory_on_numa_node();
}

numa_tutorial::~numa_tutorial() {
  if(initialized) 
    hwloc_topology_destroy(topology_);
}
