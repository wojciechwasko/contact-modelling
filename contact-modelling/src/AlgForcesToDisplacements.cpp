#include "AlgForcesToDisplacements.hpp"

#include <stdexcept>

#include "MeshInterface.hpp"
#include "external/armadillo.hpp"
#include "helpers/string.hpp"

typedef arma::mat   precomputed_type;
using helpers::string::sb;

boost::any
AlgForcesToDisplacements::impl_offline(
  const MeshInterface& forces,
  const MeshInterface& disps,
  const boost::any& params
)
{
  if (disps.D != 1 && disps.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements matrix: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  if (forces.D != 1 && forces.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces matrix: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

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
  if (disps.D != 1 && disps.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements matrix: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  if (forces.D != 1 && forces.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces matrix: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  // const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
  // const params_type& p = boost::any_cast<const params_type&>(params);
  //
  // perform update, possibly using non-const output.getRawValues() and Armadillo's
  // conv_to<std::vector<double>>
}
