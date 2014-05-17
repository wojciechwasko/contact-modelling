#include "cm/algorithm/displacements_to_forces.hpp"

#include <stdexcept>

#include "cm/mesh/interface.hpp"

#include "cm/details/elastic_model_boussinesq.hpp"
#include "cm/details/string.hpp"
#include "cm/details/external/armadillo.hpp"

namespace cm {
using details::sb;

typedef arma::mat   precomputed_type;

boost::any AlgDisplacementsToForces::impl_offline(
  const MeshInterface& disps,
  const MeshInterface& forces,
  const boost::any& params
)
{
  if (disps.D != 1 && disps.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements mesh: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  if (forces.D != 1 && forces.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces mesh: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  const params_type& p = boost::any_cast<const params_type&>(params);
  using cm::details::displacements_to_forces_matrix;
  return displacements_to_forces_matrix(disps, forces, p.skin_props);
}

void AlgDisplacementsToForces::impl_run(
  const MeshInterface& disps,
        MeshInterface& forces,
  const boost::any& params,
  const boost::any& precomputed
)
{
  if (disps.D != 1 && disps.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements mesh: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  if (forces.D != 1 && forces.D != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces mesh: "
            << disps.D << "; supported dimensionalities: (1,3)"
    );

  const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
  arma::colvec temp = pre * arma::conv_to<arma::colvec>::from(disps.getRawValues());
  forces.getRawValues().assign(temp.begin(), temp.end());
}

} /* namespace cm */
