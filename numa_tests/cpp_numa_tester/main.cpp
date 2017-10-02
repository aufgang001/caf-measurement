#include <iostream>
#include <vector>

//#include "hwloc_cpp_helper.hpp"
#include "tester.hpp"

using namespace std;
using namespace std::chrono;

const size_t one_gb= 1*1024*1024*1024; //1Gbyte

string data_rate_to_string(size_t rate_bytes_per_sec) {
  string result(to_string(rate_bytes_per_sec / (1 * 1024 * 1024)));  
  result += " MBytes/s";
  return result;
}

vector<tester> prepare_testers(hwloc_topology_t topo, int num_threads,
                               size_t memory_size,
                               hwloc_const_bitmap_t node_set_a,
                               hwloc_const_bitmap_t node_set_b) {
  vector<tester> ts;
  for (auto i = 0; i < num_threads; ++i) {
    auto tmp_node_set_a = hwloc_bitmap_make_wrapper();
    auto tmp_node_set_b = hwloc_bitmap_make_wrapper();
    hwloc_bitmap_copy(tmp_node_set_a.get(), node_set_a);
    hwloc_bitmap_copy(tmp_node_set_b.get(), node_set_b);
    ts.emplace_back(topo, move(tmp_node_set_a), move(tmp_node_set_b),
                    memory_size);
  }
  return ts;
}

void run_testers(vector<tester>& ts, seconds test_duration) {
  for (auto& t: ts) {
    t.start_tester(); 
  }
  for (auto& t: ts) {
    t.start_measurement(); 
  }
  this_thread::sleep_for(test_duration);
  for (auto& t: ts) {
    t.stop_measurement(); 
  }
  for (auto& t: ts) {
    t.stop_tester(); 
  }
  size_t data_rate_sum = 0;
  for (auto& t: ts) {
    data_rate_sum += t.get_data_rate();
    cout << "data rate: " << t.get_data_rate() << endl;
  }
  cout << "sum data rate: " << data_rate_to_string(data_rate_sum) << endl;
}


int main(int /*argc*/, char** /*argv[]*/) {
  auto topo = hwloc_make_topology_wrapper();
  auto node_set = hwloc_bitmap_make_wrapper();
  hwloc_bitmap_set(node_set.get(), 1);
  auto testers = prepare_testers(topo.get(), 8, one_gb, node_set.get(), node_set.get());
  run_testers(testers, seconds(10));
}
