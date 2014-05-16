#ifndef ELASTIC_MODEL_LOVE_HPP
#define ELASTIC_MODEL_LOVE_HPP

#include "cm/details/external/armadillo.hpp"

namespace cm {

class MeshInterface;
class MeshRectangularBase;
class SkinAttributes;

namespace details {

/**
 * \brief   Calculate a matrix, which post-multiplied by the pressures vector will yield the
 *          displacements vector.
 * \param  p    pressures mesh. Required for length and locations and such
 * \tparam d    displacements mesh. Required for number of nodes, locations, etc.
 */
arma::mat pressures_to_displacements_matrix(
  const MeshRectangularBase& p,
  const MeshInterface& d,
  const SkinAttributes& skin_attr
);

/**
 * \brief   Calculate a matrix, which post-multiplied by the displacements vector will yield the
 *          pressures vector. (in least RMSE sense)
 * \param  p    pressures mesh. Required for length and locations and such
 * \tparam d    displacements mesh. Required for number of nodes, locations, etc.
 *
 * Basically returns a pinv of pressures_to_displacements_matrix()
 */
arma::mat displacements_to_pressures_matrix(
  const MeshInterface& d,
  const MeshRectangularBase& p,
  const SkinAttributes& skin_attr
);

namespace impl {

/**
 * \sa  sanity_checks_forces_to_displacements
 */
void
sanity_checks_pressures_to_displacements(
  const MeshRectangularBase& p,
  const MeshInterface& d
);

/**
 * \brief   Calculate influence coefficient for Love(1929) problem -- multiplied by pressure value
 *          yields the displacement value in z direction.
 * \param   dx,dy   0.5*dimensions of the pressure cell; the pressure cell extends from -dx to +dx
 *                  in x direction and from -dy to +dy in y direction
 * \param   E       Young's modulus
 * \param   nu      Poisson's ratio
 * \param   x,y,z   Coordinates of the point inside the solid where to compute the displacement,
 *                  relative to the pressure cell's origin
 *
 * Solutions are due to Janet Becker and Michael Bevis (doi: 10.1111/j.1365-246X.2003.02150.x)
 * http://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?2004GeoJI.156..171B
 */
double love_coeff(
  const double dx, const double dy,
  const double E, const double nu,
  const double x, const double y, const double z
);

namespace love {

/**
 * \brief   Helper function. Calculates the Lj coefficient (eqn. (39) in Becker's paper)
 * \tparam  j   either 1 or 2. Will not compile for other values
 * \param   yp  y' (as in: y prime)
 * \note    For other parameters, see love_coeff()
 */
template <size_t j>
double Lj(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
);

/**
 * \brief   Helper function. Calculates the rj0 coefficient (eqn. (5) in Becker's paper)
 * \sa      For detailed description see Lj()
 */
template <size_t j>
double rj0(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
);

/**
 * \brief   Helper function. Calculates the betaj0 coefficient (eqn. (5) in Becker's paper)
 * \sa      For detailed description see Lj()
 */
template <size_t j>
double betaj0(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
);

/**
 * \brief   Helper function. Calculates the rj0 coefficient (eqn. (5) in Becker's paper)
 * \sa      For detailed description see Lj()
 */
template <size_t j>
double psij0(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
);

} /* namespace love */
} /* namespace impl */
} /* namespace details */
} /* namespace cm */


#endif /* ELASTIC_MODEL_LOVE_HPP */
