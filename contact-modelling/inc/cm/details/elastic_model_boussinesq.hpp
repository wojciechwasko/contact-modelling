#ifndef ELASTIC_MODEL_BOUSSINESQ_HPP
#define ELASTIC_MODEL_BOUSSINESQ_HPP

#include "cm/details/external/armadillo.hpp"

/**
 * \cond DEV
 */

/**
 * \file 
 * \brief   Implementation of the Boussinesq-Cerruti model (point forces)
 */

namespace cm {

class Grid;
class GridRectangularBase;
class SkinAttributes;

namespace details {

/**
 * \brief   Calculate a matrix, which post-multiplied by the forces vector will yield the
 *          displacements vector.
 * \param  f    forces grid. Required for length and locations and such
 * \tparam d    displacements grid. Required for number of cells, locations, etc.
 */
arma::mat forces_to_displacements_matrix(
  const Grid& f,
  const Grid& d,
  const SkinAttributes& skin_attr
);

/**
 * \brief   Calculate a matrix, which post-multiplied by the displacements vector will yield the
 *          forces vector. (in least RMSE sense)
 * \param  f    forces grid. Required for length and locations and such
 * \tparam d    displacements grid. Required for number of cells, locations, etc.
 *
 * Basically returns a pinv of forces_to_displacements_matrix()
 */
arma::mat displacements_to_forces_matrix(
  const Grid& d,
  const Grid& f,
  const SkinAttributes& skin_attr
);

/**
 * \brief   Some even more hidden implementation details.
 */
namespace impl {

/**
 * \brief   Runtime checks for creating the forces to displacements matrix.
 *
 * For the first issue found, this function will throw a std::runtime_error with an
 * appropriate message.
 *
 * Sometimes, this check could be done at compile time, but the moment we introduce
 * dynamically (e.g. SkinWare-) generated grid or a Rectangular grid spanning it, we enter the
 * dynamic realm.
 */
void sanity_checks_forces_to_displacements(
  const Grid& f,
  const Grid& d
);

} /* namespace impl */
} /* namespace details */
} /* namespace cm */


/**
 * \endcond
 */

#endif /* ELASTIC_MODEL_BOUSSINESQ_HPP */
