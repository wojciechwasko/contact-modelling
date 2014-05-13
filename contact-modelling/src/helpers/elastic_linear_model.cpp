#include "helpers/elastic_linear_model.hpp"

#include <cmath>
#include <stdexcept>

#include "SkinAttributes.hpp"
#include "MeshInterface.hpp"
#include "MeshRegularRectangular.hpp"
#include "helpers/math.hpp"
#include "helpers/string.hpp"
#include "helpers/log.hpp"

using helpers::string::sb;

namespace helpers {
  namespace elastic_linear_model {
    arma::mat 
    forces_to_displacements_matrix(
      const MeshInterface& f,
      const MeshInterface& d,
      const SkinAttributes& skin_attr
    )
    {
      using helpers::math::eq_almost;
      impl::sanity_checks_forces_to_displacements(f,d);
    
      const size_t f_stride = f.D;
      const size_t d_stride = d.D;
      const size_t nrows = d.no_nodes() * d_stride;        // number of rows: displacements 
      const size_t ncols = f.no_nodes() * f_stride;        // number of cols: forces
    
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
    
      for (size_t nj = 0; nj < f.no_nodes(); ++nj) {
        for (size_t ni = 0; ni < d.no_nodes(); ++ni) {
          dx = d.node(ni).x - f.node(nj).x;   dx2 = dx*dx;
          dy = d.node(ni).y - f.node(nj).y;   dy2 = dy*dy;
          z0h = sqrt(f.node_area(nj) * 3.0 / (2*pi));
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
    
    arma::mat
    displacements_to_forces_matrix(
      const MeshInterface& d,
      const MeshInterface& f,
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

    arma::mat
    pressures_to_displacements_matrix(
      const MeshRegularRectangular& p,
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

    arma::mat
    displacements_to_pressures_matrix(
      const MeshInterface& d,
      const MeshRegularRectangular& p,
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
      )
      {
        if (f.D != 3 && f.D != 1) {
          throw std::runtime_error(
            "F's nodes have an unsupported dimensionality: " +
            (sb() << f.D).str()
          );
        }
      
        if (d.D != 3 && d.D != 1) {
          throw std::runtime_error("D's nodes have an unsupported dimensionality.");
        }
      
        if (f.D == 3 && f.getRawValues().size() % 3 != 0) {
          throw std::runtime_error("F's nodes have a dimensionality of 3 "
              "but the number of values is not divisible by 3.");
        }
      
        if (d.D == 3 && d.getRawValues().size() % 3 != 0) {
          throw std::runtime_error("D's nodes have a dimensionality of 3 "
              "but the number of values is not divisible by 3.");
        }
      }
    
      void
      sanity_checks_pressures_to_displacements(
        const MeshRegularRectangular& p,
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
        using helpers::math::eq_almost;
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
          using helpers::math::eq_almost;
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
          using helpers::math::eq_almost;
    
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
      }
    }
  }
}
