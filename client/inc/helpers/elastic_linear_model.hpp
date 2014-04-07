#ifndef LINEAR_MODEL_HPP
#define LINEAR_MODEL_HPP

#include <stdexcept>
#include <type_traits>

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

//        if (F::node_type::D != 3 && F::node_type::D != 1)
//          throw std::runtime_error("F's nodes have an unsupported dimensionality.");
//        if (D::node_type::D != 3 && D::node_type::D != 1)
//          throw std::runtime_error("D's nodes have an unsupported dimensionality.");
//
//        if (F::node_type::D == 3 && f_vals.size() % 3 != 0)
//          throw std::runtime_error("F's nodes have a dimensionality of 3, but the number of values is not "
//              "divisible by 3.");
      }
    };

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
      arma::mat ret;
      const typename F::values_container& f_vals = f.getRawValues();
      const typename D::values_container& d_vals = d.getRawValues();
      impl::sanity_checks_forces_to_displacements(f,d);

      // TODO fill the matrix
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
