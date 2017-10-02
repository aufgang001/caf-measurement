#include "tester.hpp"

#include <chrono>
#include <cstring>

tester::tester(hwloc_topology_t topo, bitmap_wrapper_t local_node_set,
               bitmap_wrapper_t remote_node_set, size_t memory_size)
    : topo_(topo),
      local_node_set_(move(local_node_set)),
      remote_node_set_(move(remote_node_set)),
      memory_size_(memory_size),
      local_data_(nullptr, hwloc_mem_deposer(topo, 0)),
      remote_data_(nullptr, hwloc_mem_deposer(topo, 0)),
      running_(true),
      data_rate_(0) {
  init();
}

void tester::init() {
  remote_data_.reset(
    hwloc_alloc_membind_policy(topo_, memory_size_, remote_node_set_.get(),
                               HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_THREAD));
  if (remote_data_.get() == nullptr) {
    std::cerr << "hwloc_alloc_membind_policy() for remote memory failed" << std::endl;
    exit(EXIT_SUCCESS);
  }
  local_data_.reset(
    hwloc_alloc_membind_policy(topo_, memory_size_, local_node_set_.get(),
                               HWLOC_MEMBIND_BIND, HWLOC_MEMBIND_THREAD));
  if (local_data_.get() == nullptr) {
    std::cerr << "hwloc_alloc_membind_policy() for local memory failed" << std::endl;
    exit(EXIT_SUCCESS);
  }
}

void tester::run_measurement() {
  using namespace std::chrono;
  auto bind_cpu_set = hwloc_bitmap_make_wrapper();
  hwloc_cpuset_from_nodeset(topo_, bind_cpu_set.get(), local_node_set_.get());
  auto err = hwloc_set_cpubind(topo_, bind_cpu_set.get(),
                               HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND);
  if (err == -1) {
    std::cerr << "hwloc_set_cpubind() failed" << std::endl;
    exit(EXIT_SUCCESS);
  }
  auto start = std::chrono::high_resolution_clock::now();
  size_t iterations = 0;
  while(running_.load()) {
    memcpy(local_data_.get(), remote_data_.get(), memory_size_);
    ++iterations;
  }
  auto end = high_resolution_clock::now();
  duration<double> diff = end-start;
  std::cout << "iterations: " << iterations << std::endl;
  std::cout << "diff.count(): " << diff.count() << std::endl;
  data_rate_ = (memory_size_ * iterations) / diff.count();
}

void tester::start_tester() {
  thread_ = std::thread(&tester::run_measurement, this);
}

void tester::stop_tester() {
  running_.store(false); 
  thread_.join();
}

void tester::start_measurement() {

}

void tester::stop_measurement() {

}

// in bytes per seconds
size_t tester::get_data_rate() {
  return data_rate_;
}
