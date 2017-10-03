#ifndef TESTER_HPP
#define TESTER_HPP

#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include <cstring>

#include "hwloc_cpp_helper.hpp"

template<class T>
class tester {
public:
  using meta_data_t = T;

  tester(hwloc_topology_t topo, bitmap_wrapper_t thread_node_set,
         bitmap_wrapper_t dst_mem_node_set, bitmap_wrapper_t src_mem_node_set,
         size_t memory_size)
      : topo_(topo),
        thread_node_set_(move(thread_node_set)),
        dst_mem_node_set_(move(dst_mem_node_set)),
        src_mem_node_set_(move(src_mem_node_set)),
        memory_size_(memory_size),
        dst_data_(nullptr, hwloc_mem_disposer(topo, memory_size)),
        src_data_(nullptr, hwloc_mem_disposer(topo, memory_size)),
        running_(new std::atomic<bool>(true)),
        measuring_(new std::atomic<bool>(false)),
        copy_rate_(0) {
    init();
  }

  tester() = delete;
  tester(tester&&) = default;
  tester& operator=(tester&&) = default;
  tester(const tester&) = delete;
  tester& operator=(const tester&) = delete;

  void start_tester() {
    // ATTENTION!! after executing this function, tester cannot be coppied
    // anymore
    thread_ = std::thread(&tester::run_measurement, this);
  }

  void stop_tester() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void start_measurement() {
    measuring_->store(true);
  }

  void stop_measurement() {
    measuring_->store(false);
    running_->store(false);
  }

  // in bytes per seconds
  size_t get_copy_rate() {
    return copy_rate_;
  }

  meta_data_t meta_data;
private:
  using mem_t = std::unique_ptr<void, hwloc_mem_disposer>;

  void allocate_node_specific_mem(mem_t& mem, bitmap_wrapper_t& from) {
    mem.reset(hwloc_alloc_membind_nodeset(
      topo_, memory_size_, from.get(), HWLOC_MEMBIND_BIND,
      HWLOC_MEMBIND_THREAD | HWLOC_MEMBIND_STRICT));
    if (mem.get() == nullptr) {
      std::cerr << "hwloc_alloc_membind_nodeset() failed" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void pin_this_thread(bitmap_wrapper_t& to) {
    auto bind_cpu_set = hwloc_bitmap_make_wrapper();
    hwloc_cpuset_from_nodeset(topo_, bind_cpu_set.get(), to.get());
    auto err =
      hwloc_set_cpubind(topo_, bind_cpu_set.get(),
                        HWLOC_CPUBIND_THREAD | HWLOC_CPUBIND_NOMEMBIND);
    if (err == -1) {
      std::cerr << "hwloc_set_cpubind() failed" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void init() {
    allocate_node_specific_mem(dst_data_, dst_mem_node_set_);
    allocate_node_specific_mem(src_data_, src_mem_node_set_);
    std::cout << "mem a: " << dst_mem_node_set_ << std::endl;
    std::cout << "mem b: " << src_mem_node_set_ << std::endl;
  }

  void run_measurement() {
    using namespace std::chrono;
    pin_this_thread(thread_node_set_);
    std::cout << "pin thread: " << thread_node_set_ << std::endl;
    while (running_->load() && !measuring_->load()) {
    }
    if (measuring_->load()) {
      auto start = std::chrono::high_resolution_clock::now();
      size_t iterations = 0;
      while (running_->load()) {
        memcpy(dst_data_.get(), src_data_.get(), memory_size_);
        ++iterations;
      }
      auto end = high_resolution_clock::now();
      duration<double> diff = end - start;
      copy_rate_ = (memory_size_ * iterations) / diff.count();
    }
  }

  hwloc_topology_t topo_;
  bitmap_wrapper_t thread_node_set_;
  bitmap_wrapper_t dst_mem_node_set_;
  bitmap_wrapper_t src_mem_node_set_;
  size_t memory_size_; // in bytes
  mem_t dst_data_;
  mem_t src_data_;
  std::unique_ptr<std::atomic<bool>> running_;
  std::unique_ptr<std::atomic<bool>> measuring_;
  std::thread thread_;
  size_t copy_rate_; //bytes per seconds
};



#endif //TESTER_HPP
