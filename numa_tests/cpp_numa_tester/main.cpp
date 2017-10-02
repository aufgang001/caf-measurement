#include <iostream>

//#include "hwloc_cpp_helper.hpp"
#include "tester.hpp"

using namespace std;

int main(int /*argc*/, char** /*argv[]*/) {
  auto topo = hwloc_make_topology_wrapper();
  auto node_set_a = hwloc_bitmap_make_wrapper();
  auto node_set_b = hwloc_bitmap_make_wrapper();
  hwloc_bitmap_set(node_set_a.get(), 1);
  hwloc_bitmap_set(node_set_b.get(), 1);
  tester t(topo.get(), move(node_set_a), move(node_set_b), 10);
  t.start_tester();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  t.stop_tester();
}
