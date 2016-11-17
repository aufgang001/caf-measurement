#include <iostream>
#include "hierarchical_scheduler_topo.hpp"

//http://www.open-mpi.de/projects/hwloc/doc/v1.11.4/a00097.php
hierarchical_scheduler_topo::hierarchical_scheduler_topo(
  hwloc_topology_t topology, unsigned int num_of_lvls, unsigned int current_cpu_id)
    : topology_(topology)
{
  std::vector<unsigned int> cpus_up_to_current_lvl;
  auto max_lvl = hwloc_topology_get_depth(topology_) - 1;

  for (unsigned int lvl = max_lvl; lvl > num_of_lvls; --lvl) {
    
  }


}

const std::vector<unsigned int>& hierarchical_scheduler_topo::get_cpus(size_t lvl) {
  return cpu_per_lvls_[lvl];
}


