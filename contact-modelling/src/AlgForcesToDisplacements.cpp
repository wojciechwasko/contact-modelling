#include "cm/algorithm/forces_to_displacements.hpp"

#include <stdexcept>

#include "cm/mesh/interface.hpp"
#include "cm/details/external/armadillo.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_boussinesq.hpp"

namespace cm {
using details::sb;

typedef arma::mat   precomputed_type;

boost::any AlgForcesToDisplacements::impl_offline(
  const MeshInterface& forces,
  const MeshInterface& disps,
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
  using cm::details::forces_to_displacements_matrix;
  return forces_to_displacements_matrix(forces, disps, p.skin_props);
}

void AlgForcesToDisplacements::impl_run(
  const MeshInterface& forces,
        MeshInterface& disps,
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

  const precomputed_type& pre = boost::any_cast<const precomputed_type&>(precomputed);
  arma::colvec temp = pre * arma::conv_to<arma::colvec>::from(forces.getRawValues());
  disps.getRawValues().assign(temp.begin(), temp.end());
}

} /* namespace cm */
