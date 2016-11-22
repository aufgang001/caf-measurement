#include "hierarchical_scheduler_topo.hpp"

#include <iostream>
#include <map>

hierarchical_scheduler_topo::hierarchical_scheduler_topo(
  const hwloc_topology_t topology)
    : topo_(topology)
    , distance_matrix_(get_hwloc_distance_matrix()) {
}

hierarchical_scheduler_topo::pu_matrix_t
hierarchical_scheduler_topo::get_pu_matrix(const pu_set_t& current_pu_id_set) const {
  pu_matrix_t result_matrix;
  auto current_node_set = get_node_set(current_pu_id_set);  
  auto error = false;
  if (distance_matrix_ == nullptr || distance_matrix_->latency == nullptr) {
    // info msg !!!
    error = true; 
  }
  if (hwloc_bitmap_iszero(current_node_set.get())) {
    // fehler meldung !!!
    error = true; 
  }
  // If no matrix is provides or an other error occures
  // it is asumed that all PUs (Processing Units) have the same distance.
  if (error) {
    pu_set_t tmp_pu_set = hwloc_bitmap_wrapper(
      hwloc_bitmap_dup(hwloc_topology_get_topology_cpuset(topo_)));
    hwloc_bitmap_andnot(tmp_pu_set.get(), tmp_pu_set.get(),
                        current_pu_id_set.get());
    //result_matrix.emplace_back(std::move(tmp_pu_set));
    result_matrix.push_back(std::move(tmp_pu_set));
    return result_matrix;
  }
  // Create result matrix
  node_id_t current_node_id = hwloc_bitmap_first(current_node_set.get());
  unsigned int num_of_dist_objs = distance_matrix_->nbobjs;
  float* dist_pointer =
    &distance_matrix_->latency[num_of_dist_objs * current_node_id];
  std::map<float, pu_set_t> dist_map;
  for (unsigned int idx = 0; idx < num_of_dist_objs; ++idx) {
    node_set_t tmp_node_set = hwloc_bitmap_make_wrapper();
    hwloc_bitmap_set(tmp_node_set.get(), idx);
    pu_set_t tmp_pu_set = get_pu_set(tmp_node_set);
    auto dist_it = dist_map.find(dist_pointer[idx]);
    if (dist_it == dist_map.end())
      dist_map.insert(std::make_pair(dist_pointer[idx], std::move(tmp_pu_set)));
    else
      hwloc_bitmap_or(dist_it->second.get(), dist_it->second.get(),
                      tmp_pu_set.get());
  }
  for (auto& it : dist_map) {
    result_matrix.emplace_back(std::move(it.second));
  }
  return result_matrix;
}

hierarchical_scheduler_topo::node_set_t
hierarchical_scheduler_topo::get_node_set(const pu_set_t& pu_set) const {
  node_set_t node_set = hwloc_bitmap_make_wrapper();
  hwloc_cpuset_to_nodeset(topo_, pu_set.get(), node_set.get());
  return node_set;
}

hierarchical_scheduler_topo::pu_set_t
hierarchical_scheduler_topo::get_pu_set(const node_set_t& node_set) const {
  pu_set_t pu_set = hwloc_bitmap_make_wrapper();
  hwloc_cpuset_from_nodeset(topo_, pu_set.get(), node_set.get());
  return pu_set;
}

const hwloc_distances_s*
hierarchical_scheduler_topo::get_hwloc_distance_matrix() const {
  return hwloc_get_whole_distance_matrix_by_type(topo_, HWLOC_OBJ_NUMANODE);
}
