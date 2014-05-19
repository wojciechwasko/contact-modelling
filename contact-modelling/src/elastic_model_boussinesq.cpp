#include "cm/details/elastic_model_boussinesq.hpp"

#include <cmath>
#include <stdexcept>

#include "cm/skin/attributes.hpp"
#include "cm/grid/grid.hpp"

#include "cm/details/math.hpp"
#include "cm/details/string.hpp"
#include "cm/log/log.hpp"

namespace cm {
namespace details {

arma::mat forces_to_displacements_matrix(
  const Grid& f,
  const Grid& d,
  const SkinAttributes& skin_attr
)
{
  using cm::details::eq_almost;
  if (!eq_almost(skin_attr.nu, 0.5, 1e-3)) {
    LOG(WARN) << "The equations implemented for the forces-to-displacements model are only valid for nu=0.5";
  }
  impl::sanity_checks_forces_to_displacements(f,d);

  const size_t f_stride = f.dim();
  const size_t d_stride = d.dim();
  const size_t nrows = d.num_cells() * d_stride;        // number of rows: displacements 
  const size_t ncols = f.num_cells() * f_stride;        // number of cols: forces

  // helper variables, not to be computed at each iteration
  const double eps_samepoint      = 1e-5;
  const double pi = M_PI;
  const double E = skin_attr.E;
  const double h = skin_attr.h;
  const double h2 = h*h;
  const double coeff_samepoint_xy = 9.0/(4 * pi * E);
  const double coeff_samepoint_z  = 9.0/(2 * pi * E);
  const double coeff_other        = 3.0/(4 * pi * E);
  const double psi                = 0.25; // Note this should be a function of h/zoh

  arma::mat ret(nrows, ncols);
  // a series of lambdas to faciliate filling the matrix.
  // Notation:
  //    xx | xy | xz
  //    yx | yy | yz
  //    zx | zy | zz
  // that corresponds to the (3D forces, 3D displacements) case
  //
  // If we consider (3D forces, 1D (z-only) displacements), the following lambdas will be used:
  //    zx | zy | zz
  //
  // If we consider (1D (z-only) forces, 3D displacements), the following lambdas will be used:
  //    xz
  //    yz
  //    zz
  //
  // Finally, if we consider (1D forces, 1D displacements), obviously, only the "zz" lambda will
  // be used.
  //
  // The chunk written to is [ni*d_stride ... ni*d_stride+d_stride-1] x [nj*f_stride .. nj*f_stride+f_stride-1] (rows x cols)
  //
  // Variable variables accessed by lamdas (through reference closure); will be set at each
  // iteration of the for loops
  bool is_same;
  double dx, dx2;
  double dy, dy2;
  double z0h;
  double coeff_inv_dxdy;
  double coeff_inv_dxdy_pow3;
  double coeff_inv_dxdyh_pow3;
  // The lambdas; 
  auto write_xx = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      coeff_samepoint_xy * psi / z0h
    : 
      coeff_other * (
          coeff_inv_dxdy_pow3 *    (2*dx2 + dy2)
        - coeff_inv_dxdyh_pow3 * (2*dx2 + dy2 + h2)
      )
    ;
  };

  auto write_xy = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      0
    : 
      coeff_other * (
          coeff_inv_dxdy_pow3 *    (dx * dy)
        - coeff_inv_dxdyh_pow3 * (dx * dy)
      )
    ;
  }; 

  auto write_xz = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      0
    : 
      coeff_other * (
        - coeff_inv_dxdyh_pow3 *    (dx * h)
      )
    ;
  };

  auto write_yx = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      0
    : 
      coeff_other * (
          coeff_inv_dxdy_pow3 *    (dx * dy)
        - coeff_inv_dxdyh_pow3 * (dx * dy)
      )
    ;
  };

  auto write_yy = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      coeff_samepoint_xy * psi / z0h
    : 
      coeff_other * (
          coeff_inv_dxdy_pow3 *    (dx2 + 2*dy2)
        - coeff_inv_dxdyh_pow3 * (dx2 + 2*dy2 + h2)
      )
    ;
  };

  auto write_yz = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      0
    : 
      coeff_other * (
        - coeff_inv_dxdyh_pow3 *    (dy * h)
      )
    ;
  };

  auto write_zx = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      0
    : 
      coeff_other * (
        - coeff_inv_dxdyh_pow3 *    (dx *  h)
      )
    ;
  };

  auto write_zy = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      0
    : 
      coeff_other * (
        - coeff_inv_dxdyh_pow3 *    (dy *  h)
      )
    ;
  }; 

  auto write_zz = [&](size_t row, size_t col) {
    ret(row,col) = (is_same) ? 
      coeff_samepoint_z * psi / z0h 
    : 
      coeff_other * (
          coeff_inv_dxdy
        - coeff_inv_dxdyh_pow3 *    (dx2 + dy2 + 2*h2)
      )
    ;
  }; 

  for (size_t nj = 0; nj < f.num_cells(); ++nj) {
    for (size_t ni = 0; ni < d.num_cells(); ++ni) {
      dx = d.cell(ni).x - f.cell(nj).x;   dx2 = dx*dx;
      dy = d.cell(ni).y - f.cell(nj).y;   dy2 = dy*dy;
      z0h = sqrt(f.getCellShape().area() * 3.0 / (2*pi));
      is_same = eq_almost(dx, 0, eps_samepoint) && eq_almost(dy, 0, eps_samepoint);
      coeff_inv_dxdy       = 1.0 / sqrt(dx2 + dy2);
      coeff_inv_dxdy_pow3  = pow(coeff_inv_dxdy, 3);
      coeff_inv_dxdyh_pow3 = pow(1.0 / sqrt(dx2 + dy2 + h2), 3);

      if (f_stride == 1) {
        if (d_stride == 1) {
          // forces are only normal, displacements only normal
          write_zz(ni*d_stride + 0, nj*f_stride + 0);
        } else {
          // forces are only normal, displacements are in 3D
          write_xz(ni*d_stride + 0, nj*f_stride + 0);
          write_yz(ni*d_stride + 1, nj*f_stride + 0);
          write_zz(ni*d_stride + 2, nj*f_stride + 0);
        }
      } else {
        if (d_stride == 1) {
          // forces are in 3D, displacements only normal
          write_zx(ni*d_stride + 0, nj*f_stride + 0);
          write_zy(ni*d_stride + 0, nj*f_stride + 1);
          write_zz(ni*d_stride + 0, nj*f_stride + 2);
        } else {
          // forces are in 3D, displacements are in 3D
          write_xx(ni*d_stride + 0, nj*f_stride + 0);
          write_xy(ni*d_stride + 0, nj*f_stride + 1);
          write_xz(ni*d_stride + 0, nj*f_stride + 2);

          write_yx(ni*d_stride + 1, nj*f_stride + 0);
          write_yy(ni*d_stride + 1, nj*f_stride + 1);
          write_yz(ni*d_stride + 1, nj*f_stride + 2);

          write_zx(ni*d_stride + 2, nj*f_stride + 0);
          write_zy(ni*d_stride + 2, nj*f_stride + 1);
          write_zz(ni*d_stride + 2, nj*f_stride + 2);
        }
      }
    }
  }
  IFLOG(DEBUG2) {
    static size_t sn = 0;
    std::string fname = sb() << "fd-" << sn << ".csv";
    LOG(DEBUG2) << "Saving the forces-to-displacements matrix to '" << fname << "'.";
    ret.save(fname, arma::csv_ascii);
    ++sn;
  }
  return ret;
}
    
arma::mat displacements_to_forces_matrix(
  const Grid& d,
  const Grid& f,
  const SkinAttributes& skin_attr
)
{
  arma::mat orig = forces_to_displacements_matrix(f,d,skin_attr);
  arma::mat ret = arma::pinv(orig);
  IFLOG(DEBUG2) {
    static size_t sn = 0;
    std::string fname_orig = sb() << "orig-fd-" << sn << ".csv";
    LOG(DEBUG2) << "Saving the original (before pseudoinverse) matrix to '" << fname_orig << "'.";
    orig.save(fname_orig, arma::csv_ascii);
    std::string fname_pinv = sb() << "pinv-fd-" << sn << ".csv";
    LOG(DEBUG2) << "Saving the pseudoinverse matrix to '" << fname_pinv << "'.";
    ret.save(fname_pinv, arma::csv_ascii);
    ++sn;
  }
  return ret;
}

namespace impl {

/**
 * \brief   Runtime checks for creating the forces to displacements matrix.
 *
 * For the first issue found, this function will throw a std::runtime_error with an
 * appropriate message.
 *
 * Sometimes, those check could be done at compile time, but the moment we introduce
 * dynamically (e.g. SkinWare-) generated grid or a Rectangular grid spanning it, we enter the
 * dynamic realm.
 */
void
sanity_checks_forces_to_displacements(
  const Grid& f,
  const Grid& d
)
{
  if (f.dim() != 3 && f.dim() != 1) {
    throw std::runtime_error(
      "F's cells have an unsupported dimensionality: " +
      (sb() << f.dim()).str()
    );
  }

  if (d.dim() != 3 && d.dim() != 1) {
    throw std::runtime_error("D's cells have an unsupported dimensionality.");
  }

  if (f.dim() == 3 && f.getRawValues().size() % 3 != 0) {
    throw std::runtime_error("F's cells have a dimensionality of 3 "
        "but the number of values is not divisible by 3.");
  }

  if (d.dim() == 3 && d.getRawValues().size() % 3 != 0) {
    throw std::runtime_error("D's cells have a dimensionality of 3 "
        "but the number of values is not divisible by 3.");
  }
}

} /* namespace impl */
} /* namespace details */
} /* namespace cm */
