#ifndef LINEAR_MODEL_HPP
#define LINEAR_MODEL_HPP

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <type_traits>

#include "helpers/math.hpp"
#include "external/armadillo.hpp"

namespace helpers {
  namespace elastic_linear_model {
    /**
     * \brief   Parameters of skin required for the linear model
     */
    typedef struct skin_properties {
      /**
       * \brief   The Young modulus of the elastomer
       */
      double elasticModulus;
      /**
       * \brief   Thickness of the layer of elastomer
       */
      double skinThickness;
    } skin_properties;

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
      template <class F, class D>
      void sanity_checks_forces_to_displacements(
        const F& f,
        const D& d
      )
      {
        const typename F::values_container& f_vals = f.getRawValues();
        const typename D::values_container& d_vals = d.getRawValues();

        if (F::D != 3 && F::D != 1)
          throw std::runtime_error("F's nodes have an unsupported dimensionality.");

        if (D::D != 3 && D::D != 1)
          throw std::runtime_error("D's nodes have an unsupported dimensionality.");

        if (F::D == 3 && f_vals.size() % 3 != 0)
          throw std::runtime_error("F's nodes have a dimensionality of 3 "
              "but the number of values is not divisible by 3.");

        if (D::D == 3 && d_vals.size() % 3 != 0)
          throw std::runtime_error("D's nodes have a dimensionality of 3 "
              "but the number of values is not divisible by 3.");
      }
    }

    /**
     * \brief   Calculate a matrix, which post-multiplied by the forces vector will yield the
     *          displacements vector.
     * \tparam  F     type of the Forces container. Must support ::node_type and iteration over
     *                nodes (to get .x and .y coordinates), preferably by indices; Also,
     *                area(node_index) for the approximate solution given by (20) in the Muscari et
     *                al. 2013 paper.
     * \tparam  D     type of the Displacements container. Must support ::node_type and iteration
     *                over nodes (to get .x and .y coordinates)
     */
    template <class F, class D>
    arma::mat forces_to_displacements_matrix(
      const F& f,
      const D& d,
      const skin_properties& skin_attr
    )
    {
      using helpers::math::eq_almost;
      impl::sanity_checks_forces_to_displacements(f,d);

      const size_t f_stride = F::D;
      const size_t d_stride = D::D;
      const size_t nrows = d.no_nodes() * d_stride;        // number of rows: displacements 
      const size_t ncols = f.no_nodes() * f_stride;        // number of cols: forces

      // helper variables, not to be computed at each iteration
      const double eps_samepoint      = 1e-4;
      const double pi = M_PI;
      const double E = skin_attr.elasticModulus;
      const double h = skin_attr.skinThickness;
      const double h2 = h*h;
      const double coeff_samepoint_xy = 9.0/(4 * pi * E);
      const double coeff_samepoint_z  = 9.0/(2 * pi * E);
      const double coeff_other        = 1.0/(4 * pi * E);
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
          - coeff_other * (
            coeff_inv_dxdyh_pow3 *    (dx * h)
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
          - coeff_other * (
            coeff_inv_dxdyh_pow3 *    (dy * h)
          )
        ;
      };

      auto write_zx = [&](size_t row, size_t col) {
        ret(row,col) = (is_same) ? 
          0
        : 
          - coeff_other * (
            coeff_inv_dxdyh_pow3 *    (dx *  h)
          )
        ;
      };

      auto write_zy = [&](size_t row, size_t col) {
        ret(row,col) = (is_same) ? 
          0
        : 
          - coeff_other * (
            coeff_inv_dxdyh_pow3 *    (dy *  h)
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
          dx = d.node(nj).x - f.node(ni).x;   dx2 = dx*dx;
          dy = d.node(nj).y - f.node(ni).y;   dy2 = dy*dy;
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
      return ret;
    }

    /**
     * \brief   Calculate a matrix, which post-multiplied by the displacements vector will yield the
     *          forces vector. (in least RMSE sense)
     * \tparam  D     type of the Displacements container. Must support ::node_type and iteration
     *                over nodes (to get .x and .y coordinates)
     * \tparam  F     type of the Forces container. Must support ::node_type and iteration over
     *                nodes (to get .x and .y coordinates), preferably by indices; Also,
     *                area(node_index) for the approximate solution given by (20) in the Muscari et
     *                al. 2013 paper.
     *
     * Basically returns a pinv of forces_to_displacements_matrix()
     */
    template <class D, class F>
    arma::mat displacements_to_forces_matrix(
      const D& d,
      const F& f,
      const skin_properties& skin_attr
    )
    {
      return arma::pinv(forces_to_displacements_matrix(f,d,skin_attr));
    }
  }
}


#endif /* LINEAR_MODEL_HPP */
