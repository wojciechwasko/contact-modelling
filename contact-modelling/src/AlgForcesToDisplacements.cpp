#include "AlgForcesToDisplacements.hpp"

#include "external/armadillo.hpp"

typedef arma::mat   precomputed_type;

boost::any
AlgForcesToDisplacements::impl_offline(
  const MeshInterface& forces,
  const MeshInterface& disps,
  const boost::any& params
)
{
  const params_type& p = boost::any_cast<const params_type&>(params);
  using helpers::elastic_linear_model::forces_to_displacements_matrix;
  return forces_to_displacements_matrix(forces, disps, p.skin_props);
}

void
AlgForcesToDisplacements::impl_run(
  const MeshInterface& forces,
        MeshInterface& disps,
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
