#include "cm/algorithm/pressures_to_displacements.hpp"

#include <stdexcept>
#include <typeinfo>

#include "cm/grid/grid.hpp"
#include "cm/details/external/armadillo.hpp"
#include "cm/details/string.hpp"
#include "cm/details/elastic_model_love.hpp"

namespace cm {
using details::sb;

typedef arma::mat   precomputed_type;

boost::any AlgPressuresToDisplacements::impl_offline(
  const Grid& pressures,
  const Grid& disps,
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
  using cm::details::pressures_to_displacements_matrix;
  return pressures_to_displacements_matrix(pressures, disps, p.skin_props);
}

void AlgPressuresToDisplacements::impl_run(
  const Grid& pressures,
        Grid& disps,
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

  const precomputed_type& pre = boost::any_cast<const precomputed_type&>(precomputed);
  arma::colvec temp = pre * arma::conv_to<arma::colvec>::from(pressures.getRawValues());
  disps.getRawValues().assign(temp.begin(), temp.end());
}

} /* namespace cm */
