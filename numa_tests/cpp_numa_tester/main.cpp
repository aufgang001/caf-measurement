#include <iostream>
#include <vector>

#include "test_manager.hpp"

using namespace std;

void print_helper_txt(test_manager& tm) {

  std::cout << "./numa_tester <option> " << std::endl
    << "   Option" << std::endl
    << "     [-h | --help]     print this help test" << std::endl
    << "     [test_mode]       run a specific test" << std::endl
    << std::endl;
  tm.plot_available_test_modes();
}

int main(int argc, char** argv) {
  constexpr size_t option_idx = 1;
  test_manager t("tester.ini");
  if (argc != 2) {
    print_helper_txt(t);
  } else if (std::string(argv[option_idx]) == "-h"
             || std::string(argv[option_idx]) == "--help") {
    print_helper_txt(t); 
  } else {
     t.run(argv[option_idx]);
  }
}

