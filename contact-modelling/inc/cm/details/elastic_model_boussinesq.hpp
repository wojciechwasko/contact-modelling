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
 * \param psi_exact   whether to calculate the Psi function for the approximate
 *                    solution using the exact formula or just set it to 0.25
 */
arma::mat forces_to_displacements_matrix(
  const Grid& f,
  const Grid& d,
  const SkinAttributes& skin_attr,
  const bool  psi_exact
);

/**
 * \brief   Calculate a matrix, which post-multiplied by the displacements vector will yield the
 *          forces vector. (in least RMSE sense)
 * \param d    displacements grid. Required for number of cells, locations, etc.
 * \param f    forces grid. Required for length and locations and such
 * \param psi_exact   whether to calculate the Psi function for the approximate
 *                    solution using the exact formula or just set it to 0.25
 *
 * Basically returns a pinv of forces_to_displacements_matrix()
 */
arma::mat displacements_to_forces_matrix(
  const Grid& d,
  const Grid& f,
  const SkinAttributes& skin_attr,
  const bool  psi_exact
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

struct CoeffsBouss {
  /**
   * \brief   Precalculate the various coefficients which are commonly used
   * \param   E   Elastic (Young's) modulus of the skin
   * \param   x   distance between the points along the x axis
   * \param   y   distance between the points along the y axis
   * \param   h   thickness of the skin
   * \param   s   area of the discretisation element (for approximate solution)
   * \param   psi_exact   whether to use the exact formula (true) or just set
   *                      Psi to 0.25, as propose in Muscari et al.;
   *                      default: true
   */
  CoeffsBouss(
    const double E,
    const double x,
    const double y,
    const double h,
    const double s,
    const bool   psi_exact = true
  );
  /**
   * \brief Eq: $\frac{3}{4\pi E}$
   */
  const double c_3_4_pi_E;
  /**
   * \brief Eq: $\frac{3}{4\pi E} \frac{1}{(\sqrt{x^2+y^2})}$
   */
  const double c_3_4_pi_E_xy;
  /**
   * \brief Eq: $\frac{3}{4\pi E} \frac{1}{(\sqrt{x^2+y^2})^3}$
   */
  const double c_3_4_pi_E_xy3;
  /**
   * \brief Eq: $\frac{3}{4\pi E} \frac{1}{(\sqrt{x^2+y^2+h^2})^3}$
   */
  const double c_3_4_pi_E_xyh3;
  /**
   * \brief eq: $\frac{9}{4\pi E} \frac{Psi(h/z0h)}{z0h}$
   */
  const double c_9_4_pi_E_psi_z0h;
  /**
   * \brief eq: $\frac{9}{2\pi E} \frac{Psi(h/z0h)}{z0h}$
   */
  const double c_9_2_pi_E_psi_z0h;
};

/**
 * \brief   Calculate the 1D-forces -to- 1D-displacements matrix
 */
arma::mat f2d_11(const Grid& f, const Grid& d, const SkinAttributes& skin_attr);

/**
 * \brief   Calculate the 1D-forces -to- 3D-displacements matrix
 */
arma::mat f2d_13(const Grid& f, const Grid& d, const SkinAttributes& skin_attr);

/**
 * \brief   Calculate the 3D-forces -to- 1D-displacements matrix
 */
arma::mat f2d_31(const Grid& f, const Grid& d, const SkinAttributes& skin_attr);

/**
 * \brief   Calculate the 3D-forces -to- 3D-displacements matrix
 */
arma::mat f2d_33(const Grid& f, const Grid& d, const SkinAttributes& skin_attr);

/**
 * \brief   Calculate function Psi(x), as described in the thesis of Luca Muscari
 *
 * The formula reads $\Psi(x) = \frac{0.2431 \cdot x - 0.1814}{x}$.
 */
double psi(const double v);

/**
 * \brief   Calculate z0h from the area a force affects
 *
 * The formula reads $z_{0h} = \sqrt{\frac{3}{2\pi}\Delta s}$ where $\Delta s$
 * is the area a force is exerted upon (we take it to be equal to the area of a
 * single forces' grid's cell.
 */
double z0h(const double Ds);

/**
 * \name  Exact solution for Boussinesq's model. Singluar at x=y=0
 *
 * Those functions return the influence coefficients corresponding to "pure"
 * Boussinesq-Cerruti solution.
 *
 * `skin_attr` are parameters of the skin we're running on
 * `x,y` aredistances between the point of application of the force and the
 * point in which the displacement is measured; along respective axes, i.e. x =
 * displacement.x - force.x
 */

/**\{*/
double bouss_xx(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);
double bouss_xy(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);
double bouss_xz(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);

double bouss_yx(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);
double bouss_yy(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);
double bouss_yz(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);

double bouss_zx(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);
double bouss_zy(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);
double bouss_zz(const SkinAttributes& skin_attr, const CoeffsBouss& cb,const double x, const double y);

double appro_xx(const SkinAttributes& sa, const CoeffsBouss& cb);
double appro_yy(const SkinAttributes& sa, const CoeffsBouss& cb);
double appro_zz(const SkinAttributes& sa, const CoeffsBouss& cb);

/**\}*/

} /* namespace impl */
} /* namespace details */
} /* namespace cm */


/**
 * \endcond
 */

#endif /* ELASTIC_MODEL_BOUSSINESQ_HPP */
