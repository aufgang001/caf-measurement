#include <iostream>
#include "hwloc_bitmap_wrapper.hpp"

hwloc_bitmap_wrapper::hwloc_bitmap_wrapper()
    : bmap_(hwloc_bitmap_alloc())
{
}

hwloc_bitmap_wrapper::hwloc_bitmap_wrapper(const std::string& str) 
    : hwloc_bitmap_wrapper()
{
  set(str);
}

hwloc_bitmap_wrapper::hwloc_bitmap_wrapper(const hwloc_bitmap_t& other)
    : hwloc_bitmap_wrapper() {
    hwloc_bitmap_copy(bmap_, other);
}

hwloc_bitmap_wrapper::hwloc_bitmap_wrapper(const hwloc_bitmap_wrapper& other)
    : hwloc_bitmap_wrapper(other.cget())
{
}

hwloc_bitmap_wrapper& hwloc_bitmap_wrapper::
operator=(const hwloc_bitmap_wrapper& other) {
  hwloc_bitmap_copy(bmap_, other.cget());
  return *this;
}

bool hwloc_bitmap_wrapper::operator==(const hwloc_bitmap_wrapper& other) const {
  return hwloc_bitmap_isequal(bmap_, other.cget());
}

hwloc_bitmap_t& hwloc_bitmap_wrapper::get() {
  return bmap_;
}

const hwloc_bitmap_t& hwloc_bitmap_wrapper::cget() const {
  return bmap_;
}

void hwloc_bitmap_wrapper::set(const std::string& str) {
    hwloc_bitmap_sscanf(bmap_, str.c_str());
}

std::ostream& operator <<(std::ostream& s, const hwloc_bitmap_wrapper& bmap) {
  char *str;
  hwloc_bitmap_asprintf(&str, bmap.cget());
  s << std::string(str);
  free(str);
  return s;
}

void hwloc_bitmap_wrapper::set(unsigned int id) {
  hwloc_bitmap_set(bmap_, id);
}

void hwloc_bitmap_wrapper::unset(unsigned int id) {
  hwloc_bitmap_clr(bmap_, id);
}

void hwloc_bitmap_wrapper::singlify() {
  hwloc_bitmap_singlify(bmap_);
}

bool hwloc_bitmap_wrapper::is_set(unsigned int id) const {
  return hwloc_bitmap_isset(bmap_, id);
}

hwloc_bitmap_wrapper::~hwloc_bitmap_wrapper() {
  hwloc_bitmap_free(bmap_);
}

void hwloc_bitmap_wrapper::test() {
  using namespace std;
  cout << "##-- Test hwloc_bitmap_wrapper --##" << endl;

  cout << "TEST 1" << endl;    
  {
    hwloc_bitmap_wrapper{};
  }
  cout << "TEST 2" << endl;    
  {
    hwloc_bitmap_wrapper x;
    hwloc_bitmap_wrapper y;
    x = y;
  }
  cout << "TEST 3" << endl;    
  {
    hwloc_bitmap_wrapper x;
    x = hwloc_bitmap_wrapper{};
  }
  cout << "TEST 4" << endl;    
  {
    hwloc_bitmap_wrapper x;
    hwloc_bitmap_wrapper y(x);
  }
  cout << "TEST 5" << endl;    
  {
    hwloc_bitmap_wrapper x;
    hwloc_bitmap_wrapper y;
    x = move(y);
  }
  cout << "TEST 6" << endl;    
  {
    hwloc_bitmap_wrapper x = hwloc_bitmap_wrapper{};
  }
}
