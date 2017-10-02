#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>
#include <thread>
#include <atomic>

#include "hwloc_cpp_helper.hpp"

class tester {
public:
  tester(hwloc_topology_t topo, bitmap_wrapper_t local_node_set,
         bitmap_wrapper_t remote_node_set, size_t memory_size);
  tester() = delete;

  tester(tester&&) = default;
  tester& operator=(tester&&) = default;

  tester(const tester&) = delete;
  tester& operator=(const tester&) = delete;

  void start_tester();
  void stop_tester();

  void start_measurement();
  void stop_measurement();

  // in bytes per seconds
  size_t get_data_rate();
  

private:
  void init();
  
  void run_measurement();

  hwloc_topology_t topo_;
  bitmap_wrapper_t local_node_set_;
  bitmap_wrapper_t remote_node_set_;
  size_t memory_size_; // in bytes
  std::unique_ptr<void, hwloc_mem_deposer> local_data_;
  std::unique_ptr<void, hwloc_mem_deposer> remote_data_;
  std::unique_ptr<std::atomic<bool>> running_;
  std::unique_ptr<std::atomic<bool>> measuring_;
  std::thread thread_;
  size_t data_rate_; //bytes per seconds
};


#endif // TESTER_HPP
