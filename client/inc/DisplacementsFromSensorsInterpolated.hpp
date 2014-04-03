#ifndef DISPLACEMENTSFROMSENSORSINTERPOLATED_HPP
#define DISPLACEMENTSFROMSENSORSINTERPOLATED_HPP

#include <memory>
#include <type_traits>
#include <stdexcept>

#include "traits_helpers.hpp"

#include "MeshInterface.hpp"
#include "MeshNatural.hpp"
#include "MeshNode.hpp"

#include "InterpolatorInterface.hpp"


/**
 * \brief   A class to represent displacements (deflections?) read from the senors
 * \tparam  target_mesh_type    Type of mesh the values will be interpolated onto. Note that the
 *                              dimensionality of the values must be == 1
 * \tparam  interpolator        Algorithm used to interpolate values from one mesh onto another.
 *
 */
template <
  class SkinConnector,
  class target_mesh_type,
  class interpolator_type
>
class DisplacementsFromSensorsInterpolated {
  // asserts
  static_assert(
    target_mesh_type::node_type::D == 1,
    "For DisplacementsFromSensorsInterpolated, the dimensionality of the target mesh "
    "must be equal to 1. (Those are the sensors we're using)"
  );

    typedef MeshNatural<MeshNode<1>, SkinConnector> source_mesh_type;
    SkinConnector* skin_conn_;
    std::unique_ptr<source_mesh_type>   source_mesh_;
    std::unique_ptr<target_mesh_type>   target_mesh_;
    std::unique_ptr<interpolator_type> interpolator_;
  public:
    typedef typename target_mesh_type::values_container  values_container;
    typedef typename target_mesh_type::node_type      node_type;

    /**
     * \brief   Constructor for interpolated mesh.
     *
     * This constructor will be used if we need to interpolate the sensory readings. We have
     * a mesh determined by the physical skin sensors layout and then we have another mesh
     * with "virtual" readigs which come from interpolation by some specific metehod.
     */
    DisplacementsFromSensorsInterpolated(
      SkinConnector& skin_conn,
      std::unique_ptr<target_mesh_type> target_mesh
    ) : 
      skin_conn_(&skin_conn),
      source_mesh_(new source_mesh_type(skin_conn.sensors_begin(), skin_conn.sensors_end())),
      target_mesh_(
        (target_mesh) ?
          std::move(target_mesh) :
          throw std::runtime_error("DisplacementsFromSensorsInterpolated: passed empty target mesh unique_ptr!")
      ),
      interpolator_(new interpolator_type(source_mesh_.get(), target_mesh_.get()))
    {
      // FIXME  check if this static_assert can be rewritten to be more general.
      //        I don't forsee any other MeshNode types, but if they become necessary,
      //        it'll be necessary to add them to this static_assert.
      static_assert(
           !std::is_same<target_mesh_type, MeshNatural<MeshNode<1>, SkinConnector> >::value
        && !std::is_same<target_mesh_type, MeshNatural<MeshNode<3>, SkinConnector> >::value,
        "You cannot use the mesh-interpolation constructor on a MeshNatural!\n"
        "NOTE: It doesn't make sense, because there's no interpolation for a natural mesh."
      );
    }

    const source_mesh_type& getSourceMesh() const
    {
      return *source_mesh_;
    }

    const target_mesh_type& getTargetMesh() const
    {
      return *target_mesh_;
    }

    /**
     * \brief   Get a constant reference to the values vector.
     * \note    In the interpolated case, this corresponds to the interpolated
     *          values.
     */
    const values_container& getRawValues() const { return target_mesh_->getRawValues(); };

    /**
     * \brief   Get a reference to the values vector.
     * \note    In the interpolated case, this corresponds to the interpolated
     *          values.
     */
    values_container& getRawValues() { return target_mesh_->getRawValues(); };

    void update()
    {
      skin_conn_->update(source_mesh_->getRawValues());
      interpolator_->interpolateBulk();
    }
};


#endif /* DISPLACEMENTSFROMSENSORSINTERPOLATED_HPP */
