#include "cm/algorithm/displacements_to_pressures.hpp"

#include <stdexcept>

#include "cm/grid/grid.hpp"
#include "cm/details/external/armadillo.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_love.hpp"

namespace cm {
using details::sb;

/**
 * \cond DEV
 */
namespace details {
typedef arma::mat   precomputed_type;
}
/**
 * \endcond
 */

boost::any AlgDisplacementsToPressures::impl_offline(
  const Grid& disps,
  const Grid& pressures,
  const boost::any& params
)
{
  if (disps.dim() != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements grid: "
            << disps.dim() << "; supported dimensionalities: (1,)"
    );

  if (pressures.dim() != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the pressures grid: "
            << disps.dim() << "; supported dimensionalities: (1,)"
    );

  const params_type& p = boost::any_cast<const params_type&>(params);
  using cm::details::displacements_to_pressures_matrix;
  return displacements_to_pressures_matrix(disps, pressures, p.skin_props);
}

void AlgDisplacementsToPressures::impl_run(
  const Grid& disps,
        Grid& pressures,
  const boost::any& params,
  const boost::any& precomputed
)
{
  if (disps.dim() != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the displacements grid: "
            << disps.dim() << "; supported dimensionalities: (1,)"
    );

  if (pressures.dim() != 1)
    throw std::runtime_error(
      sb()  << "Wrong dimensionality of the pressures grid: "
            << disps.dim() << "; supported dimensionalities: (1,)"
    );

  const details::precomputed_type& pre = boost::any_cast<details::precomputed_type>(precomputed);
  std::vector<double> tmp = arma::conv_to<std::vector<double>>::from(
      pre * arma::conv_to<arma::colvec>::from(disps.getRawValues())
    );
  pressures.setRawValues(
     std::move(tmp) 
  );
}

} /* namespace cm */
