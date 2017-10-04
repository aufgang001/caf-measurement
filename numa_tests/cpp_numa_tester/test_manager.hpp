#ifndef TEST_MANAGER_HPP
#define TEST_MANAGER_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <type_traits>
#include <iomanip>

#include "hwloc_cpp_helper.hpp"
#include "tester.hpp"
#include "config_map.hpp"

using namespace std::chrono;

std::string data_rate_to_string(size_t rate_bytes_per_sec) {
  std::string result(std::to_string(rate_bytes_per_sec / (1 * 1024 * 1024)));  
  return result;
}

constexpr size_t one_gb= 1*1024*1024*1024; //1Gbyte

class test_manager {
public:
  test_manager(std::string ini_file) {
    configuration_.read_ini(ini_file);
    available_tests_.insert(std::make_pair(
      "single", [this](const std::string& m) { run_test_single(m); }));
    available_tests_.insert(std::make_pair(
      "matrix", [this](const std::string& m) { run_test_matrix(m); }));
  }

  void plot_available_test_modes() {
    std::cout << "Available test modes"  << std::endl;
    for (auto& group: configuration_) {
      std::cout << " - " << group.first << std::endl;
    } 
  } 

  struct meta_data {
    
  };

  using tester_t = tester<meta_data>;
  using testers_t = std::vector<tester_t>;

  void run(const std::string& test_mode) {
    auto it = available_tests_.find(test_mode);
    if (it != available_tests_.end()) {
      it->second(move(test_mode));    
    } else {
      std::cerr << "Error! Test mode: " << test_mode << " not available." << std::endl;
    }
  }
private:
  template<class U, class Enable = void>
  struct get_config_value { };

  template <class U>
  struct get_config_value<U, typename std::
                               enable_if<std::is_integral<U>::value>::type> {
    get_config_value(const config_map& c) : configuration_(c) {
    }
    U operator()(const std::string& test_mode, const std::string& key) {
      auto res = configuration_.get(test_mode, key);
      if (res.empty()) {
        std::cerr << "ERROR! configuration: (" << test_mode << ", " << key
                  << ") not available" << std::endl;
        exit(EXIT_FAILURE);
      }
      return static_cast<U>(stoll(res));
    }
    const config_map& configuration_;
  };

  template <class U>
  struct get_config_value<U,
                          typename std::enable_if<std::is_same<U, std::string>::
                                                    value>::type> {
    get_config_value(const config_map& c) : configuration_(c) {
    }
    U operator()(const std::string& test_mode, const std::string& key) {
      auto res = configuration_.get(test_mode, key);
      if (res.empty()) {
        std::cerr << "ERROR! configuration: (" << test_mode << ", " << key
                  << ") not available" << std::endl;
        exit(EXIT_FAILURE);
      }
      return res;
    }
    const config_map& configuration_;
  };

  void run_test_matrix(const std::string& test_mode) {
    using namespace std;
    auto& c = configuration_;
    auto num_of_threads = get_config_value<int>(c)(test_mode, "num_of_threads");
    auto test_duration = get_config_value<int>(c)(test_mode, "duration");
    size_t memory_size =
      one_gb * get_config_value<size_t>(c)(test_mode, "memory_size");
    auto topo = hwloc_make_topology_wrapper();
    auto tmp_node_set = hwloc_topology_get_allowed_nodeset(topo.get());
    testers_t testers;
    std::vector<bitmap_wrapper_t> nodes;
    for (int i = hwloc_bitmap_first(tmp_node_set); i != -1;
         i = hwloc_bitmap_next(tmp_node_set, i)) {
      auto tmp_set = hwloc_bitmap_make_wrapper();
      hwloc_bitmap_set(tmp_set.get(), i);
      nodes.emplace_back(move(tmp_set));
    }
    // create matrix
    testers.reserve(nodes.size()*nodes.size());
    for (auto& local_node: nodes) {
      for (auto& remote_node: nodes) {
        auto tmp_ts =
          prepare_testers(topo.get(), num_of_threads, local_node.get(),
                          local_node.get(), remote_node.get(), memory_size);
        for (auto& t: tmp_ts) {
          testers.emplace_back(std::move(t)); 
        }
      }
    }
    // run  matrix
    run_testers(testers, seconds(test_duration));
    // plot matrix
    size_t header_space = 3;
    size_t number_space = 7;
    //   plot header
    cout << setw(header_space) << left << "";
    for (auto& remote_node : nodes) {
      int local_node_id = hwloc_bitmap_first(remote_node.get());
      cout << setw(number_space) << right << local_node_id;
    }
    cout << endl;
    //   plot content
    auto current_tester_it = testers.begin();
    for (auto& local_node: nodes) {
      int local_node_id = hwloc_bitmap_first(local_node.get());
      cout << setw(header_space) << left << local_node_id;
      for (auto& remote_node: nodes) {
        cout << setw(number_space) << right << data_rate_to_string(current_tester_it->get_copy_rate());
        ++current_tester_it;
      }
      cout << endl;
    }
  } 

  void run_test_single(const std::string& test_mode) {
    auto& c = configuration_;
    auto thread_node_id = get_config_value<int>(c)(test_mode, "thread_node_id");
    auto dst_mem_node_id = get_config_value<int>(c)(test_mode, "dst_mem_node_id");
    auto src_mem_node_id = get_config_value<int>(c)(test_mode, "src_mem_node_id");
    auto num_of_threads = get_config_value<int>(c)(test_mode, "num_of_threads");
    auto test_duration = get_config_value<int>(c)(test_mode, "duration");
    size_t memory_size =
      one_gb * get_config_value<size_t>(c)(test_mode, "memory_size");
    auto topo = hwloc_make_topology_wrapper();
    auto thread_node_set = hwloc_bitmap_make_wrapper();
    auto dst_mem_node_set = hwloc_bitmap_make_wrapper();
    auto src_mem_node_set = hwloc_bitmap_make_wrapper();
    hwloc_bitmap_set(thread_node_set.get(), thread_node_id);
    hwloc_bitmap_set(dst_mem_node_set.get(), dst_mem_node_id);
    hwloc_bitmap_set(src_mem_node_set.get(), src_mem_node_id);
    auto testers = prepare_testers(
      topo.get(), num_of_threads, thread_node_set.get(), dst_mem_node_set.get(),
      src_mem_node_set.get(), memory_size);
    run_testers(testers, seconds(test_duration));
    sum_tester_results(testers);
  } 

  testers_t prepare_testers(hwloc_topology_t topo, int num_threads,
                                         hwloc_const_bitmap_t thread_node_set,
                                         hwloc_const_bitmap_t dst_mem_node_set,
                                         hwloc_const_bitmap_t src_mem_node_set,
                                         size_t memory_size) {
    testers_t ts;
    for (auto i = 0; i < num_threads; ++i) {
      auto tmp_thread_node_set = hwloc_bitmap_make_wrapper();
      auto tmp_dst_mem_node_set = hwloc_bitmap_make_wrapper();
      auto tmp_src_mem_node_set = hwloc_bitmap_make_wrapper();
      hwloc_bitmap_copy(tmp_thread_node_set.get(), thread_node_set);
      hwloc_bitmap_copy(tmp_dst_mem_node_set.get(), dst_mem_node_set);
      hwloc_bitmap_copy(tmp_src_mem_node_set.get(), src_mem_node_set);
      ts.emplace_back(topo, move(tmp_thread_node_set),
                      move(tmp_dst_mem_node_set), move(tmp_src_mem_node_set),
                      memory_size);
    }
    return ts;
  }

  void run_testers(testers_t& ts, seconds test_duration) {
    for (auto& t : ts) {
      t.start_tester();
    }
    for (auto& t : ts) {
      t.start_measurement();
    }
    std::this_thread::sleep_for(test_duration);
    for (auto& t : ts) {
      t.stop_measurement();
    }
    for (auto& t : ts) {
      t.stop_tester();
    }
  }

  void sum_tester_results(testers_t& ts) {
    size_t copy_rate_sum = 0;
    for (auto& t : ts) {
      copy_rate_sum += t.get_copy_rate();
    }
    std::cout << "sum copy rate: " << data_rate_to_string(copy_rate_sum) << " MBytes/s" << std::endl;
  }

  config_map configuration_;
  std::map<std::string, std::function<void(const std::string&)>> available_tests_;
};

#endif // TEST_MANAGER_HPP
