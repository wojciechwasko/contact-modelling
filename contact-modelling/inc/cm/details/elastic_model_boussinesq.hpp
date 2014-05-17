#ifndef ELASTIC_MODEL_BOUSSINESQ_HPP
#define ELASTIC_MODEL_BOUSSINESQ_HPP

#include "cm/details/external/armadillo.hpp"

namespace cm {

class MeshInterface;
class MeshRectangularBase;
class SkinAttributes;

namespace details {

/**
 * \brief   Calculate a matrix, which post-multiplied by the forces vector will yield the
 *          displacements vector.
 * \param  f    forces mesh. Required for length and locations and such
 * \tparam d    displacements mesh. Required for number of nodes, locations, etc.
 */
arma::mat forces_to_displacements_matrix(
  const MeshInterface& f,
  const MeshInterface& d,
  const SkinAttributes& skin_attr
);

/**
 * \brief   Calculate a matrix, which post-multiplied by the displacements vector will yield the
 *          forces vector. (in least RMSE sense)
 * \param  f    forces mesh. Required for length and locations and such
 * \tparam d    displacements mesh. Required for number of nodes, locations, etc.
 *
 * Basically returns a pinv of forces_to_displacements_matrix()
 */
arma::mat displacements_to_forces_matrix(
  const MeshInterface& d,
  const MeshInterface& f,
  const SkinAttributes& skin_attr
);

namespace impl {

/**
 * \brief   Runtime checks for creating the forces to displacements matrix.
 *
 * For the first issue found, this function will throw a std::runtime_error with an
 * appropriate message.
 *
 * Sometimes, those check could be done at compile time, but the moment we introduce
 * dynamically (e.g. SkinWare-) generated mesh or a Rectangular mesh spanning it, we enter the
 * dynamic realm.
 */
void
sanity_checks_forces_to_displacements(
  const MeshInterface& f,
  const MeshInterface& d
);

} /* namespace impl */
} /* namespace details */
} /* namespace cm */


#endif /* ELASTIC_MODEL_BOUSSINESQ_HPP */
