#ifndef HWLOC_BITMAP_WRAPPER_HPP
#define HWLOC_BITMAP_WRAPPER_HPP

#include <hwloc.h>
#include <string>

class hwloc_bitmap_wrapper {
public:
  hwloc_bitmap_wrapper();
  hwloc_bitmap_wrapper(const hwloc_bitmap_wrapper&);
  explicit hwloc_bitmap_wrapper(const hwloc_bitmap_t&);
  explicit hwloc_bitmap_wrapper(const std::string&);
  hwloc_bitmap_wrapper& operator=(const hwloc_bitmap_wrapper&);
  hwloc_bitmap_t& get();
  const hwloc_bitmap_t& cget() const;
  void set(unsigned int id);
  void unset(unsigned int id);
  void singlify();
  bool is_set(unsigned int id) const;
  void set(const std::string&);
  bool operator==(const hwloc_bitmap_wrapper&) const ;
  friend std::ostream& operator <<(std::ostream& s, const hwloc_bitmap_wrapper& bmap);
  ~hwloc_bitmap_wrapper();

  static void test();
private:
  hwloc_bitmap_t bmap_;
};



#endif // NUMA_TUTORIAL_HPP
