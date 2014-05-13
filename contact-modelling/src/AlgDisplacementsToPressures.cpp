#include "AlgDisplacementsToPressures.hpp"

#include <stdexcept>

#include "MeshInterface.hpp"
#include "MeshRegularRectangular.hpp"
#include "external/armadillo.hpp"
#include "helpers/string.hpp"

typedef arma::mat   precomputed_type;
using helpers::string::sb;

boost::any
AlgDisplacementsToPressures::impl_offline(
  const MeshInterface& disps,
  const MeshInterface& pressures,
  const boost::any& params
)
{
  const MeshRegularRectangular* p_mesh;
  try {
    p_mesh = dynamic_cast<const MeshRegularRectangular*>(&pressures);
  } catch (const std::bad_cast& e) {
    throw std::runtime_error(
      sb()  << "Error downcasting MeshInterface to MeshRegularRectangular. "
            << "Note: only rectangular-element-based meshes are currently supported for "
            << "pressures calculations. Original exception message: " << e.what()
    );
  }

  if (disps.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  if (pressures.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the pressures mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  const params_type& p = boost::any_cast<const params_type&>(params);
  using helpers::elastic_linear_model::displacements_to_pressures_matrix;
  return displacements_to_pressures_matrix(disps, *p_mesh, p.skin_props);
}

void
AlgDisplacementsToPressures::impl_run(
  const MeshInterface& disps,
        MeshInterface& pressures,
  const boost::any& params,
  const boost::any& precomputed
)
{
  if (disps.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  if (pressures.D != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the pressures mesh: "
            << disps.D << "; supported dimensionalities: (1,)"
    );

  const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
  arma::colvec temp = pre * arma::conv_to<arma::colvec>::from(disps.getRawValues());
  pressures.getRawValues().assign(temp.begin(), temp.end());
}
