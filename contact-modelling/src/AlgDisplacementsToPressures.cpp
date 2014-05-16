#include "cm/algorithm/displacements_to_pressures.hpp"

#include <stdexcept>

#include "cm/mesh/interface.hpp"
#include "cm/mesh/rectangular_base.hpp"
#include "cm/details/external/armadillo.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_love.hpp"

namespace cm {
using details::sb;

typedef arma::mat   precomputed_type;

boost::any AlgDisplacementsToPressures::impl_offline(
  const MeshInterface& disps,
  const MeshInterface& pressures,
  const boost::any& params
)
{
  const MeshRectangularBase* p_mesh;
  try {
    p_mesh = dynamic_cast<const MeshRectangularBase*>(&pressures);
  } catch (const std::bad_cast& e) {
    throw std::runtime_error(
      sb()  << "Error downcasting MeshInterface to MeshRectangularBase. "
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
  using cm::details::displacements_to_pressures_matrix;
  return displacements_to_pressures_matrix(disps, *p_mesh, p.skin_props);
}

void AlgDisplacementsToPressures::impl_run(
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

} /* namespace cm */
