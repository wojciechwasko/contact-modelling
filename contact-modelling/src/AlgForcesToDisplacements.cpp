#include "cm/algorithm/forces_to_displacements.hpp"

#include <stdexcept>

#include "cm/grid/grid.hpp"
#include "cm/details/external/armadillo.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_boussinesq.hpp"

namespace cm {
using details::sb;

typedef arma::mat   precomputed_type;

boost::any AlgForcesToDisplacements::impl_offline(
  const Grid& forces,
  const Grid& disps,
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
  using cm::details::forces_to_displacements_matrix;
  return forces_to_displacements_matrix(forces, disps, p.skin_props);
}

void AlgForcesToDisplacements::impl_run(
  const Grid& forces,
        Grid& disps,
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

  const precomputed_type& pre = boost::any_cast<const precomputed_type&>(precomputed);

  std::vector<double> tmp = arma::conv_to<std::vector<double>>::from(
      pre * arma::conv_to<arma::colvec>::from(forces.getRawValues())
    );
  disps.setRawValues(std::move(tmp));
}

} /* namespace cm */
