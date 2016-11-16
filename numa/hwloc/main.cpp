#include <iostream>

#include "numa_test.hpp"
#include "numa_tutorial.hpp"
#include "hwloc_bitmap_wrapper.hpp"

using namespace std;

int main(int, char**){
  //numa_tutorial n;
  //n.run_test();
  //hwloc_bitmap_wrapper::test();
  numa_test n;
  n.run_test();
}
