#include "AlgPressuresToDisplacements.hpp"

#include <stdexcept>
#include <typeinfo>

#include "MeshInterface.hpp"
#include "MeshRegularRectangular.hpp"
#include "external/armadillo.hpp"
#include "helpers/string.hpp"

typedef arma::mat   precomputed_type;
using helpers::string::sb;

boost::any
AlgPressuresToDisplacements::impl_offline(
  const MeshInterface& pressures,
  const MeshInterface& disps,
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
  using helpers::elastic_linear_model::pressures_to_displacements_matrix;
  return pressures_to_displacements_matrix(*p_mesh, disps, p.skin_props);
}

void
AlgPressuresToDisplacements::impl_run(
  const MeshInterface& pressures,
        MeshInterface& disps,
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

  const precomputed_type& pre = boost::any_cast<const precomputed_type&>(precomputed);
  arma::colvec temp = pre * arma::conv_to<arma::colvec>::from(pressures.getRawValues());
  disps.getRawValues().assign(temp.begin(), temp.end());
}
