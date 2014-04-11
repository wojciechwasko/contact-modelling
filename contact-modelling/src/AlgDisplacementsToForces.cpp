#include "AlgDisplacementsToForces.hpp"

#include "external/armadillo.hpp"

typedef arma::mat   precomputed_type;

boost::any
AlgDisplacementsToForces::impl_offline(
  const MeshInterface& disps,
  const MeshInterface& forces,
  const boost::any& params
)
{
  const params_type& p = boost::any_cast<const params_type&>(params);
  using helpers::elastic_linear_model::displacements_to_forces_matrix;
  return displacements_to_forces_matrix(disps, forces, p.skin_props);
}

void
AlgDisplacementsToForces::impl_run(
  const MeshInterface& disps,
        MeshInterface& forces,
  const boost::any& params,
  const boost::any& precomputed
)
{
  // const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
  // const params_type& p = boost::any_cast<const params_type&>(params);
  //
  // perform update, possibly using non-const output.getRawValues() and Armadillo's
  // conv_to<std::vector<double>>
}
