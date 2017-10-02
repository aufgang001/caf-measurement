#include <iostream>

#include "hwloc_cpp_helper.hpp"
#include "tester.hpp"

using namespace std;

int main(int /*argc*/, char ** /*argv[]*/)
{
  test a;
  tester b;
  std::cout << a.x << std::endl;
  std::cout << b.x.x << std::endl;

}
