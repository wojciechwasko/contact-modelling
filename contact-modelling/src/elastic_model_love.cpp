#include "cm/details/elastic_model_love.hpp"

#include <cmath>
#include <stdexcept>

#include "cm/skin/attributes.hpp"
#include "cm/mesh/interface.hpp"
#include "cm/mesh/rectangular_base.hpp"

#include "cm/details/math.hpp"
#include "cm/details/string.hpp"
#include "cm/log/log.hpp"

namespace cm {
namespace details {

arma::mat pressures_to_displacements_matrix(
  const MeshRectangularBase& p,
  const MeshInterface& d,
  const SkinAttributes& skin_attr
)
{
  impl::sanity_checks_pressures_to_displacements(p,d);
  arma::mat ret(d.getRawValues().size(), p.getRawValues().size());
  const double load_cell_dx = p.dx()/2.0;
  const double load_cell_dy = p.dy()/2.0;
  const double E = skin_attr.E;
  const double nu = skin_attr.nu;
  const double h = skin_attr.h;
  for (size_t id = 0; id < d.size(); ++id) {
    for (size_t ip = 0; ip < p.size(); ++ip) {
      const double x = d.node(id).x - p.node(ip).x;
      const double y = d.node(id).y - p.node(ip).y;
      ret(id,ip) = 
        impl::love_coeff(load_cell_dx, load_cell_dy, E, nu, x, y, 0)
        - impl::love_coeff(load_cell_dx, load_cell_dy, E, nu, x, y, h);
    }
  }
  return ret;
}

arma::mat displacements_to_pressures_matrix(
  const MeshInterface& d,
  const MeshRectangularBase& p,
  const SkinAttributes& skin_attr
)
{
  arma::mat orig = pressures_to_displacements_matrix(p,d,skin_attr);
  arma::mat ret = arma::pinv(orig);
  IFLOG(DEBUG2) {
    static size_t sn = 0;
    std::string fname_orig = sb() << "orig-dp-" << sn << ".csv";
    LOG(DEBUG2) << "Saving the original (before pseudoinverse) matrix to '" << fname_orig << "'.";
    orig.save(fname_orig, arma::csv_ascii);
    std::string fname_pinv = sb() << "pinv-pd-" << sn << ".csv";
    LOG(DEBUG2) << "Saving the pseudoinverse matrix to '" << fname_pinv << "'.";
    ret.save(fname_pinv, arma::csv_ascii);
    ++sn;
  }
  return ret;
}

namespace impl {

void
sanity_checks_pressures_to_displacements(
  const MeshRectangularBase& p,
  const MeshInterface& d
)
{
  if (p.D != 1) {
    throw std::runtime_error((sb()
      << "p's nodes have an unsupported dimensionality: "
      << p.D
      << ". Currently, only normal pressures are allowed."
    ).str());
  }

  if (d.D != 1) {
    throw std::runtime_error((sb()
      << "d's nodes have an unsupported dimensionality: "
      << d.D
      << ". Currently, only normal displacements are allowed."
    ).str());
  }
}

double love_coeff(
  const double dx, const double dy,
  const double E, const double nu,
  const double x, const double y, const double z
)
{
  using love::rj0;
  using love::Lj;
  using cm::details::eq_almost;
  const double eps = 1e-8;
  const double pi = M_PI;
  double ret = ((1-pow(nu,2))/(pi*E)) * 
    (((Lj<1>(dy,dx,dy,x,y,z) - Lj<2>(dy,dx,dy,x,y,z))) -
    ((Lj<1>(-dy,dx,dy,x,y,z) - Lj<2>(-dy,dx,dy,x,y,z))));
  if (!eq_almost(z,0,eps)) {
    ret += (z*(1+nu)/(2*pi*E)) * 
      (atan((dx-x)*(dy-y)/(z*rj0<1>(dy,dx,dy,x,y,z))) + atan((dx+x)*(dy-y)/(z*rj0<2>(dy,dx,dy,x,y,z))) -
      (atan((dx-x)*(-dy-y)/(z*rj0<1>(-dy,dx,dy,x,y,z))) + atan((dx+x)*(-dy-y)/(z*rj0<2>(-dy,dx,dy,x,y,z)))));
  }
  return ret;
}

namespace love {

// plus-minus sign choosing according to j. j == 1 -> upper sign. j == 2 -> lower sign
template <size_t j>
double choose_sign(const double upper, const double lower)
{
  static_assert(j == 1 || j == 2, "choose_sign: j must ==1 or ==2");
  return (j==1) ? upper : lower;
}

template <size_t j>
double j_pm() { return choose_sign<j>(1,-1); };
template <size_t j>
double j_mp() { return choose_sign<j>(-1,1); };

// following 4.3 of "Love's problem" paper, this function will
// return zero for coeff ~ 0 and coeff * log(arg) otherwise
double safe_log(const double coeff, const double logarg)
{
  using cm::details::eq_almost;
  const double eps = 1e-8;
  return (eq_almost(coeff,0,eps)) ? 0 : coeff * ::log(logarg);
}

template <size_t j>
double Lj(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
)
{
  static_assert(j == 1 || j == 2, "Lj: j must ==1 or ==2");
  using cm::details::eq_almost;

  const double eps = 1e-8;
  const double deltay = yp-y;

  // notation from Matlab scripts
  const double a = dx;
  const double b = dy;
  
  double ret = 0;
  ret += safe_log(deltay, (j_pm<j>()*a-x+rj0<j>(yp,a,b,x,y,z))) - deltay + safe_log(j_pm<j>()*a-x, (1+psij0<j>(yp,a,b,x,y,z))/(1-psij0<j>(yp,a,b,x,y,z)));

  if (!eq_almost(z,0,eps)) {
    double atan_arg;
    if ((j == 1 && eq_almost(x,a,eps)) || (j == 2 && eq_almost(x,-a,eps))) {
      atan_arg = psij0<j>(yp,a,b,x,y,z);
    } else {
      atan_arg = z * psij0<j>(yp,a,b,x,y,z) / (j_pm<j>()*a-x + betaj0<j>(yp,a,b,x,y,z));
    }
    ret += 2*z*atan(atan_arg);
  }
  return ret;
}

template <size_t j>
double rj0(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
)
{
  static_assert(j == 1 || j == 2, "rj0: j must ==1 or ==2");
  return sqrt(pow(dx + j_mp<j>()*x,2) + pow(yp-y,2) + pow(z,2));
}

template <size_t j>
double betaj0(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
)
{
  static_assert(j == 1 || j == 2, "betaj0: j must ==1 or ==2");
  return sqrt(pow(dx + j_mp<j>()*x,2) + pow(z,2));
}

template <size_t j>
double psij0(
  const double yp,
  const double dx, const double dy,
  const double x, const double y, const double z
)
{
  static_assert(j == 1 || j == 2, "psij0: j must ==1 or ==2");
  return (yp-y) / (rj0<j>(yp,dx,dy,x,y,z) + betaj0<j>(yp,dx,dy,x,y,z));
}
} /* namespace love */
} /* namespace impl */
} /* namespace details */
} /* namespace cm */
