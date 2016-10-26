#include <iostream>
#include <cstdlib>
#include <vector>
#include <functional>
#include <string>

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

void usage() {
  cout << "usage: measure_distance PATTERN NODEA NODEB NUMACCESS CACHESIZE RWRATE" << endl
       << "       PATTERN:    access pattern (iterate, iterate_offset, random)" << endl
       << "       NODEA:      NUMA node of the process"                         << endl
       << "       NODEB:      NUMA node for the memory"                         << endl
       << "       NUMACCESS:  numer of memory access"                           << endl
       << "       CACHESIZE:  size of the cach which should be ignored"         << endl
       << "       RWRATE:     read/write rate in percent (100 means no writes)" << endl
       << endl
       << " examples: " << endl
       << "  CSIZE=$(lscpu | grep L1 | head -n 1 | awk '{ print $3 }' | sed 's/K/*1024/g' | bc)" << endl
       << "  ./measure_distance iterate 1 64 1000000000 $CSIZE 50"
       << "  "
       << endl;
  exit(1);
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

int main(int argc, char* argv[]){
  if (argc != 7) {
    usage();
    return 1;
  }
  uint32_t cache_line = 64;
  int x = 0;
  string pattern = argv[++x];
  int node_a = stoi(argv[++x]);
  int node_b = stoi(argv[++x]);
  uint64_t num_access = stoll(argv[++x]);
  unsigned int cache_size = stoi(argv[++x]);
  unsigned int rw_rate = stoi(argv[++x]);
  size_t cache_elements = cache_size / cache_line;
  auto alloc_fun = [node_b](size_t n) -> void* {
    return numa_alloc_onnode(n, node_b);
  };
  auto free_fun = [] (void* ptr, size_t n) {
    numa_free(ptr, n);
  };
  auto mem = vector_mem(
    vector_mem_num_of_elements_for_on_gb(), 1,
    generic_allocator<uint64_t>(alloc_fun, free_fun)); 
  auto iterate_pattern = [](size_t&, uint64_t i) -> size_t {
    return i;
  };
  auto iterate_offset_pattern = [cache_elements](size_t& idx,
                                                 uint64_t) -> size_t {
    idx += cache_elements;
    return idx;
  };
  auto random_pattern = [cache_elements](size_t& idx, uint64_t i) -> size_t {
    idx += rand_array[i % rand_array_size];
    return idx;
  };
  if (pattern == "iterate")
    mem_access(num_access, mem, rw_rate, iterate_pattern);
  else if (pattern == "iterate_offset")
    mem_access(num_access, mem, rw_rate, iterate_offset_pattern);
  else if (pattern == "random")
    mem_access(num_access, mem, rw_rate, random_pattern);
  else {
    cout << "unknown access pattern " << pattern << endl;
    return 1; 
  }
}
