#include <iostream>
#include <cstdlib>
#include <vector>
#include <functional>
#include <string>
#include <iomanip>
#include <limits>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "generic_allocator.hpp"
#include "numa.h"

using namespace std;

using vector_mem = vector<uint64_t, generic_allocator<uint64_t>>;
constexpr size_t vector_mem_num_of_elements_for_on_gb() {
  return 1024 * 1024 * 1024 / sizeof(vector_mem::value_type);
}

//#include <cstdlib>
//#include <iomanip>

  //std::srand(3);

  //for (int i= 0; i< 20; ++i) {
    //cout << setw(10) << std::rand() << ", " 
         //<< setw(10) << std::rand() << ", " 
         //<< setw(10) << std::rand() << ", " 
         //<< setw(10) << std::rand() << ", " 
         //<< setw(10) << std::rand() << ", "
         //<< endl;
  //}

constexpr const uint32_t rand_array[] = {
 612121425,  953350440,  844158168,  483147985, 1205554746, 
 495649264, 1922860801, 1856883376, 1210224072,  310914940, 
2029100602, 1344681739,  378651393,  989089924,    8614858, 
 953369895,   87517201,  552076975,   21468264, 1816952841, 
 287632273, 1207815258,  126313438,  787097142,  374612515, 
1865680798,  444268468, 1119448937, 1220723885, 1886964647, 
 455690006,  351237974,  201345136, 1649823214, 1654563454, 
 437530068,  522866290, 1665914078,  662152915,  813466561, 
1876162888,  392731087, 1426619992,  531481148,   14079694, 
 149717482, 1860313922, 1897631152,   62200281, 1308236947, 
1295258048,  829830837,  936814624,   87442789,  703517399, 
1013580304,  196412186,  368498285,  569311836, 1224446897, 
  54729967,  288040571,  515919871, 1850975641,   86695435, 
1624373423,  490040308,  716882882, 1101507133,  971609877, 
 896851090,  433549295,    8370923,  504120002, 1154412950, 
1454616516, 1634681315,  959051371, 1741786242, 1884533811, 
1020545904, 2045583477, 1542059306,  190715066, 1108768853, 
1318958913, 1058331991, 1589857740, 1122546726,  689833706, 
1433067998,  971874268, 1022450906, 1145027426,  455954397, 
 286040805, 1794063756,  387091483, 1943484145, 1077180873,
};

constexpr const size_t rand_array_size = sizeof(rand_array) / 
                                         sizeof(rand_array[0]);

enum class access_pattern {
  none = 0,
  iterate = 1, 
  random = 2
};                                  

const vector<string> access_pattern_strings = { 
  "none", 
  "iterate", 
  "random"
};

std::string to_string(access_pattern pattern) {
  return access_pattern_strings[static_cast<uint8_t>(pattern)];
}

access_pattern access_pattern_from_string(const char* pattern) {
  for (size_t x = 0; x < access_pattern_strings.size(); ++x) {
    if (access_pattern_strings[x] == pattern) {
      return static_cast<access_pattern>(x); 
    }
  }  
  return access_pattern::none; 
}

void usage(int node_a, int node_b, uint64_t num_access, unsigned int rw_rate) {
  cout << "usage: measure_distance [OPTION]..." << endl
       << "" << endl
       << "  Options:" << endl
       << "    --patter=access pattern: iterate, random (default: iterate)"                         << endl
       << "    --node-a=node-ID:        NUMA node of the process (default: " << node_a << ")"       << endl
       << "    --node-b=node-ID:        NUMA node for the memory (default: " << node_b << ")"       << endl
       << "    --num-access=count:      numer of memory access (default: " << num_access << ")"     << endl
       << "    --rw-rate:               read/write rate in percent (100 means reads only, default: " << rw_rate << ")" << endl
       << "    --overload=mode:         overload the system, start for each cpu a thread,"  << endl
       << "                             mode is the distance between running task and allocated memory" << endl
       << "    --no-numa:               ignores numa functionality"                         << endl
       << endl;
  exit(0);
}

void mem_access(uint64_t num_access, 
                vector_mem& mem,
                unsigned int rw_rate,
                const function<size_t(size_t&, uint64_t)>& access_pattern) {
  volatile uint64_t tmp = 1;
  size_t idx = 0;
  for (uint64_t i = 1; i <= num_access; ++i) {
    if (i % 100 < rw_rate)
      tmp = mem[access_pattern(idx, i) % mem.size()]; //read
    else
      mem[access_pattern(idx, i) % mem.size()] = tmp; //write
  }
}

void run_measurment_distance(access_pattern pattern, int node_a, int node_b,
                             uint64_t num_access, unsigned int rw_rate,
                             bool use_numa) {
  if (use_numa)
    if (numa_run_on_node(node_a) == -1) {
      cerr << "failed to pin this task to node " << node_a << endl;
      exit(1);
    }
  auto mem = vector_mem(
    vector_mem_num_of_elements_for_on_gb(), 1,
    generic_allocator<uint64_t>(
      [node_b, use_numa](size_t n) -> void* { 
        if (use_numa)
          return numa_alloc_onnode(n, node_b); 
        else
          return malloc(n); 
      },
      [use_numa](void* ptr, size_t n) { 
        if (use_numa)
          numa_free(ptr, n); 
        else
          free(ptr);
      }));
  auto iterate_pattern = [] (size_t&, uint64_t i) -> size_t {
    return i;
  };
  auto random_pattern = [] (size_t& idx, uint64_t i) -> size_t {
    idx += rand_array[i % rand_array_size];
    return idx;
  };
  if (pattern == access_pattern::iterate)
    mem_access(num_access, mem, rw_rate, iterate_pattern);
  else if (pattern == access_pattern::random)
    mem_access(num_access, mem, rw_rate, random_pattern);
  else {
    cerr << "unknown access pattern " << to_string(pattern) << endl;
    exit(1); 
  }
}

void overload(int overload_distance, int exclude_node_a, int exclude_node_b,
              int rw_rate, access_pattern pattern, bool use_numa) {
  int num_nodes = numa_num_configured_nodes();
  if (numa_num_task_nodes() != num_nodes) {
    cout << "some numa nodes are disabled" << endl;
    exit(1);
  }

  int num_cpus = numa_num_configured_cpus();
  if (numa_num_task_cpus() != num_cpus) {
    cout << "some cpus are disabled" << endl; 
    exit(1);
  }

  int num_started_task = 0;
  for (;;)
  for (int a = 0; a < num_cpus; ++a)
  for (int b = 0; b < num_cpus; ++b)
  if (numa_distance(a, b) == overload_distance)
    if (a != exclude_node_a || b != exclude_node_b) {
      thread(run_measurment_distance, pattern, a, b,
             numeric_limits<uint64_t>::max(), rw_rate, use_numa);
      ++num_started_task;
      // started tast musst be one less then num_cpus as e.g.
      // if we have 64 cpus we want to start 63 overload tasks
      // and one measurment task
      //while(num_started_task < (num_cpus - 1)) {
      if (num_started_task >= (num_cpus - 1)) {
        return;
      }
    }
}

int main(int argc, char* argv[]) {
  access_pattern pattern = access_pattern::iterate;
  int node_a = 0;
  int node_b = 0;
  uint64_t num_access = 1000000000;
  unsigned int rw_rate = 100;
  bool use_numa = true;

  int overload_distance = -1;
  bool verbose=false;
  int c;
  while (1) {
    static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"pattern", required_argument, 0, 'p'},
      {"node-a", required_argument, 0, 'a'},
      {"node-b", required_argument, 0, 'b'},
      {"num-access", required_argument, 0, 'n'},
      {"rw-rate", required_argument, 0, 'r'},
      {"overload", required_argument, 0, 'o'},
      {"no-numa", required_argument, 0, 'x'},
      {"verbose", required_argument, 0, 'v'},
      {0, 0, 0, 0}};

    int option_index = 0;
    c = getopt_long(argc, argv, "hp:a:b:n:r:o:v", long_options, &option_index);

    // Detect the end of the options.
    if (c == -1)
      break;

    switch (c) {
      case 'h':
        usage(node_a, node_b, num_access, rw_rate);
        break;
      case 'p':
        pattern = access_pattern_from_string(optarg);
        break;
      case 'a':
        node_a = stoi(optarg);
        break;
      case 'b':
        node_b = stoi(optarg);
        break;
      case 'n':
        num_access = stoll(optarg);
        break;
      case 'r':
        rw_rate = stoi(optarg);
        break;
      case 'o':
        overload_distance = stoi(optarg);
        break;
      case 'x':
        use_numa = false;
        break;
      case 'v':
        verbose = true;
        break;
      case '?':
        cout << "unkown command" << endl;
        break;
      default:
        abort();
    }
  }
  if (verbose) {
    int width=15;
    cout << left << setw(width) << "pattern"    << ": " << to_string(pattern) << endl
         << left << setw(width) << "node-a"     << ": " << node_a << endl
         << left << setw(width) << "node-b"     << ": " << node_b << endl
         << left << setw(width) << "num-access" << ": " << num_access << endl
         << left << setw(width) << "rw-rate"    << ": " << rw_rate << endl
         << left << setw(width) << "overload"   << ": " << overload_distance << endl
         << left << setw(width) << "no-numa"    << ": " << !use_numa << endl;
  
  }
  if (numa_available() != -1) {
    if (overload_distance != -1) {
      overload(overload_distance, node_a, node_b, rw_rate, pattern, use_numa); 
    }
    run_measurment_distance(pattern, node_a, node_b, num_access, rw_rate, use_numa);
  } else {
    cout << "numa not available" << endl; 
    exit(1);
  }
}
