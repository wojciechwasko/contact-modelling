#ifndef LINEAR_MODEL_HPP
#define LINEAR_MODEL_HPP

#include "external/armadillo.hpp"

class MeshInterface;

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
      void
      sanity_checks_forces_to_displacements(
        const MeshInterface& f,
        const MeshInterface& d
      );
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
    arma::mat forces_to_displacements_matrix(
      const MeshInterface& f,
      const MeshInterface& d,
      const skin_properties& skin_attr
    );

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
    arma::mat displacements_to_forces_matrix(
      const MeshInterface& d,
      const MeshInterface& f,
      const skin_properties& skin_attr
    );
  }
}


#endif /* LINEAR_MODEL_HPP */
