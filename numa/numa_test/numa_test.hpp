#ifndef NUMA_TEST_HPP
#define NUMA_TEST_HPP

#include <iostream>
#include <string>
#include <functional>
#include <vector>

#include <numa.h>

class numa_test {
public:
  void run_test() const;
  void test_numa_parse_nodestring() const;
  void test_numa_parse_cpustring() const;
  void test_numa_set_preferred() const;
  void test_numa_set_interleave_mask() const;
  void test_memory_allocation() const;
  void test_numa_run_on_node() const;


  bool is_numa_available() const;

  std::string print_bitmask(const bitmask* b) const;

  void print_stats() const;
  friend std::ostream& operator <<(std::ostream& s, const bitmask* b);
private:
  void numa_test_string_to_bitmask(
    const std::string& title,
    std::function<bitmask*(const char*)> parse_fun) const;
};


class numa_bitmask_wrapper {
public:
  using const_iterator = std::vector<int>::const_iterator;
  numa_bitmask_wrapper(const bitmask* b){
    for (unsigned long x = 0; x < b->size; ++x) {
      if (numa_bitmask_isbitset(b, x)) {
         bits_.push_back(x);
      }
    }
  }
  const_iterator begin() const {
    return bits_.cbegin();
  }
  const_iterator end() const {
    return bits_.cend();
  }
  size_t size() const {
    return bits_.size(); 
  }
private:
  std::vector<int> bits_;
};

#endif // NUMA_TEST_HPP
