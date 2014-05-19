#include "cm/algorithm/displacements_to_forces.hpp"

#include <stdexcept>

#include "cm/grid/grid.hpp"

#include "cm/details/elastic_model_boussinesq.hpp"
#include "cm/details/string.hpp"
#include "cm/details/external/armadillo.hpp"

namespace cm {
using details::sb;

typedef arma::mat   precomputed_type;

boost::any AlgDisplacementsToForces::impl_offline(
  const Grid& disps,
  const Grid& forces,
  const boost::any& params
)
{
  if (disps.dim() != 1 && disps.dim() != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements grid: "
            << disps.dim() << "; supported dimensionalities: (1,3)"
    );

  if (forces.dim() != 1 && forces.dim() != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces grid: "
            << disps.dim() << "; supported dimensionalities: (1,3)"
    );

  const params_type& p = boost::any_cast<const params_type&>(params);
  using cm::details::displacements_to_forces_matrix;
  return displacements_to_forces_matrix(disps, forces, p.skin_props);
}

void AlgDisplacementsToForces::impl_run(
  const Grid& disps,
        Grid& forces,
  const boost::any& params,
  const boost::any& precomputed
)
{
  if (disps.dim() != 1 && disps.dim() != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements grid: "
            << disps.dim() << "; supported dimensionalities: (1,3)"
    );

  if (forces.dim() != 1 && forces.dim() != 3)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the forces grid: "
            << disps.dim() << "; supported dimensionalities: (1,3)"
    );

  const precomputed_type& pre = boost::any_cast<precomputed_type>(precomputed);
  arma::colvec temp = pre * arma::conv_to<arma::colvec>::from(disps.getRawValues());
  forces.getRawValues().assign(temp.begin(), temp.end());
}

} /* namespace cm */
