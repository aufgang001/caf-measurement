#ifndef HIERARCHICAL_SCHEDULER_TOPO_HPP
#define HIERARCHICAL_SCHEDULER_TOPO_HPP

#include <hwloc.h>
#include <vector>

class hierarchical_scheduler_topo {
public:
  hierarchical_scheduler_topo(hwloc_topology_t topology, unsigned int num_of_lvls, unsigned int current_cpu_id);
  const std::vector<unsigned int>& get_cpus(size_t lvl);
private:
  std::vector<std::vector<unsigned int>> cpu_per_lvls_;
  hwloc_topology_t topology_;
};



#endif // HIERARCHICAL_SCHEDULER_TOPO_HPP
