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

namespace impl {

CoeffsBouss::CoeffsBouss(
  const double E,
  const double x,
  const double y,
  const double h,
  const double s
)
:
  c_3_4_pi_E(3.0/(4.0 * M_PI * E)),
  c_3_4_pi_E_xy(c_3_4_pi_E / sqrt(x*x + y*y)),
  c_3_4_pi_E_xy3(c_3_4_pi_E_xy / (x*x + y*y)),
  c_3_4_pi_E_xyh3(c_3_4_pi_E / pow(x*x + y*y + h*h, 1.5)),
  c_9_4_pi_E_psi_z0h((9.0/(4*M_PI*E)) * (psi(h/z0h(s))/z0h(s))),
  c_9_2_pi_E_psi_z0h(2*c_9_4_pi_E_psi_z0h)
{

}

double psi(const double v)
{
  return (0.2431 * v - 0.1814)/v;
}

double z0h(const double Ds)
{
  return sqrt(3*Ds/(2.0*M_PI));
}

double bouss_xx(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return (2*x*x + y*y) * cb.c_3_4_pi_E_xy3 - (2*x*x + y*y + sa.h*sa.h) * cb.c_3_4_pi_E_xyh3;
}

double bouss_xy(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return (x*y) * cb.c_3_4_pi_E_xy3 - (x*y) * cb.c_3_4_pi_E_xyh3;
}

double bouss_xz(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return - (x*sa.h) * cb.c_3_4_pi_E_xyh3;
}

double bouss_yx(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return bouss_xy(sa,cb,x,y);
}

double bouss_yy(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return (x*x + 2*y*y) * cb.c_3_4_pi_E_xy3 - (x*x + 2*y*y + sa.h*sa.h) * cb.c_3_4_pi_E_xyh3;
}

double bouss_yz(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return - (y*sa.h) * cb.c_3_4_pi_E_xyh3;
}

double bouss_zx(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return bouss_xz(sa,cb,x,y);
}

double bouss_zy(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return bouss_yz(sa,cb,x,y);
}

double bouss_zz(const SkinAttributes& sa, const CoeffsBouss& cb, const double x, const double y)
{
  return cb.c_3_4_pi_E_xy - (x*x + y*y + 2*sa.h*sa.h) * cb.c_3_4_pi_E_xyh3;
}

double appro_xx(const SkinAttributes& sa, const CoeffsBouss& cb)
{
  return cb.c_9_4_pi_E_psi_z0h;
}

double appro_yy(const SkinAttributes& sa, const CoeffsBouss& cb)
{
  return cb.c_9_4_pi_E_psi_z0h;
}

double appro_zz(const SkinAttributes& sa, const CoeffsBouss& cb)
{
  return cb.c_9_2_pi_E_psi_z0h;
}

arma::mat f2d_11(const Grid& f, const Grid& d, const SkinAttributes& skin_attr)
{
  arma::mat ret(d.num_cells(), f.num_cells());
  for (size_t ind_d = 0; ind_d < d.num_cells(); ++ind_d) {
    for (size_t ind_f = 0; ind_f < f.num_cells(); ++ind_f) {
      const double x = d.cell(ind_d).x - f.cell(ind_f).x;
      const double y = d.cell(ind_d).y - f.cell(ind_f).y;
      const CoeffsBouss cb(skin_attr.E, x, y, skin_attr.h, f.getCellShape().area());
      const double a_zz = appro_zz(skin_attr, cb);
      const double b_zz = bouss_zz(skin_attr, cb, x,y);
      if ((x == 0 && y == 0) || std::fabs(a_zz) < std::fabs(b_zz)) {
        ret(ind_d, ind_f) = a_zz;
      } else {
        ret(ind_d, ind_f) = b_zz;
      }
    }
  }

  return ret;
}

arma::mat f2d_13(const Grid& f, const Grid& d, const SkinAttributes& skin_attr)
{
  arma::mat ret(3*d.num_cells(), f.num_cells());
  for (size_t ind_d = 0; ind_d < d.num_cells(); ++ind_d) {
    for (size_t ind_f = 0; ind_f < f.num_cells(); ++ind_f) {
      const double x = d.cell(ind_d).x - f.cell(ind_f).x;
      const double y = d.cell(ind_d).y - f.cell(ind_f).y;
      const CoeffsBouss cb(skin_attr.E, x, y, skin_attr.h, f.getCellShape().area());
      const double b_zz = bouss_zz(skin_attr, cb, x, y);
      const double a_zz = appro_zz(skin_attr, cb);

      if ((x == 0 && y == 0) || std::fabs(a_zz) < std::fabs(b_zz)) {
        ret(3*ind_d +0, ind_f) = 0;
        ret(3*ind_d +1, ind_f) = 0;
        ret(3*ind_d +2, ind_f) = a_zz;
      } else {
        ret(3*ind_d +0, ind_f) = bouss_xz(skin_attr, cb, x, y);
        ret(3*ind_d +1, ind_f) = bouss_yz(skin_attr, cb, x, y);
        ret(3*ind_d +2, ind_f) = b_zz;
      }
    }
  }

  return ret;
}

arma::mat f2d_31(const Grid& f, const Grid& d, const SkinAttributes& skin_attr)
{
  arma::mat ret(d.num_cells(), 3*f.num_cells());
  for (size_t ind_d = 0; ind_d < d.num_cells(); ++ind_d) {
    for (size_t ind_f = 0; ind_f < f.num_cells(); ++ind_f) {
      const double x = d.cell(ind_d).x - f.cell(ind_f).x;
      const double y = d.cell(ind_d).y - f.cell(ind_f).y;
      const CoeffsBouss cb(skin_attr.E, x, y, skin_attr.h, f.getCellShape().area());
      const double b_zz = bouss_zz(skin_attr, cb, x, y);
      const double a_zz = appro_zz(skin_attr, cb);

      if ((x == 0 && y == 0) || std::fabs(a_zz) < std::fabs(b_zz)) {
        ret(ind_d, 3*ind_f +0) = 0;
        ret(ind_d, 3*ind_f +1) = 0;
        ret(ind_d, 3*ind_f +2) = a_zz;
      } else {
        ret(ind_d, 3*ind_f +0) = bouss_zx(skin_attr, cb, x, y);
        ret(ind_d, 3*ind_f +1) = bouss_zy(skin_attr, cb, x, y);
        ret(ind_d, 3*ind_f +2) = b_zz;
      }
    }
  }

  return ret;
}

arma::mat f2d_33(const Grid& f, const Grid& d, const SkinAttributes& skin_attr)
{
  arma::mat ret(3*d.num_cells(), 3*f.num_cells());
  for (size_t ind_d = 0; ind_d < d.num_cells(); ++ind_d) {
    for (size_t ind_f = 0; ind_f < f.num_cells(); ++ind_f) {
      const double x = d.cell(ind_d).x - f.cell(ind_f).x;
      const double y = d.cell(ind_d).y - f.cell(ind_f).y;
      const CoeffsBouss cb(skin_attr.E, x, y, skin_attr.h, f.getCellShape().area());

      const double b_xx = bouss_xx(skin_attr, cb, x, y);
      const double a_xx = appro_xx(skin_attr, cb);
      if ((x == 0 && y == 0) || std::fabs(a_xx) < std::fabs(b_xx)) {
        ret(3*ind_d +0, 3*ind_f +0) = a_xx;
        ret(3*ind_d +0, 3*ind_f +1) = 0;
        ret(3*ind_d +0, 3*ind_f +2) = 0;
      } else {
        ret(3*ind_d +0, 3*ind_f +0) = b_xx;
        ret(3*ind_d +0, 3*ind_f +1) = bouss_xy(skin_attr, cb,  x, y);
        ret(3*ind_d +0, 3*ind_f +2) = bouss_xz(skin_attr, cb,  x, y);
      }

      const double b_yy = bouss_yy(skin_attr, cb, x, y);
      const double a_yy = appro_yy(skin_attr, cb);
      if ((x == 0 && y == 0) || std::fabs(a_yy) < std::fabs(b_yy)) {
        ret(3*ind_d +1, 3*ind_f +0) = 0;
        ret(3*ind_d +1, 3*ind_f +1) = a_yy;
        ret(3*ind_d +1, 3*ind_f +2) = 0;
      } else {
        ret(3*ind_d +1, 3*ind_f +0) = bouss_yx(skin_attr, cb,  x, y);
        ret(3*ind_d +1, 3*ind_f +1) = b_yy;
        ret(3*ind_d +1, 3*ind_f +2) = bouss_yz(skin_attr, cb,  x, y);
      }

      const double b_zz = bouss_zz(skin_attr, cb, x, y);
      const double a_zz = appro_zz(skin_attr, cb);
      if ((x == 0 && y == 0) || std::fabs(a_zz) < std::fabs(b_zz)) {
        ret(3*ind_d +2, 3*ind_f +0) = 0;
        ret(3*ind_d +2, 3*ind_f +1) = 0;
        ret(3*ind_d +2, 3*ind_f +2) = a_zz;
      } else {
        ret(3*ind_d +2, 3*ind_f +0) = bouss_zx(skin_attr, cb,  x, y);
        ret(3*ind_d +2, 3*ind_f +1) = bouss_zy(skin_attr, cb,  x, y);
        ret(3*ind_d +2, 3*ind_f +2) = b_zz;
      }
    }
  }

  return ret;
}

}

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

  const size_t f_dim = f.dim();
  const size_t d_dim = d.dim();

  if (1 == f_dim && 1 == d_dim) {
    return impl::f2d_11(f,d,skin_attr);
  }
  
  if (1 == f_dim && 3 == d_dim) {
    return impl::f2d_13(f,d,skin_attr);
  }
  
  if (3 == f_dim && 1 == d_dim) {
    return impl::f2d_31(f,d,skin_attr);
  }
  
  return impl::f2d_33(f,d,skin_attr);
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
