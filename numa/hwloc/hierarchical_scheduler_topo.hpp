#ifndef HIERARCHICAL_SCHEDULER_TOPO_HPP
#define HIERARCHICAL_SCHEDULER_TOPO_HPP

#include <hwloc.h>
#include <vector>
#include <functional>

#include "hwloc_bitmap_wrapper.hpp"

class hierarchical_scheduler_topo {
public:
  using pu_id_t = int;
  using node_id_t = int;
  using pu_set_t = hwloc_bitmap_wrapper;
  using node_set_t = hwloc_bitmap_wrapper;
  using pu_matrix_t = std::vector<pu_set_t>;
  hierarchical_scheduler_topo(const hwloc_topology_t topology);
  pu_matrix_t get_pu_matrix(const pu_set_t& current_pu_id_set) const;
private:
  pu_set_t get_pu_set(const node_set_t& node_set) const ;
  node_set_t get_node_set(const pu_set_t& pu_set) const;
  const hwloc_distances_s* get_hwloc_distance_matrix() const;
  const hwloc_topology_t topo_;
  const hwloc_distances_s* distance_matrix_;
};



#endif // HIERARCHICAL_SCHEDULER_TOPO_HPP
