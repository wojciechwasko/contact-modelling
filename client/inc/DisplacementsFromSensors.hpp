#ifndef DISPLACEMENTSFROMSENSORS_HPP
#define DISPLACEMENTSFROMSENSORS_HPP

#include <memory>
#include <type_traits>
#include "traits_helpers.hpp"

#include "MeshInterface.hpp"
#include "MeshNatural.hpp"
#include "MeshNodeVal1d.hpp"
#include "MeshNodeVal3d.hpp"

#include "InterpolatorInterface.hpp"


/**
 * \brief   A class to represent displacements (deflections?) read from the senors
 * \tparam  SkinSensorIterator  Type of iterator over sensors. Templation necessary since SkinWare
 *                              provides different iterators when we're iterating over sensors coming
 *                              from a patch or a region or the whole object.
 * \tparam  target_mesh_type    Optional. Type of mesh the values will be interpolated onto. By default
 *                              the sensors' values (after scaling) will be offered as-is, in the
 *                              "natural" mesh. It is however possible to interpolate them within this
 *                              class by using a custom interpolation method.
 * \tparam  interpolator        Optional. Algorithm used to interpolate values from one mesh onto
 *                              another. Used only if targetMeshType != MeshNormal
 *
 */
using traits_helpers::EnableIf;
using traits_helpers::DisableIf;
using traits_helpers::none_placeholder;
template <
  class SkinSensorIterator,
  class target_mesh_type = MeshNatural<MeshNodeVal1d>,
  class interpolator_type = none_placeholder
>
class DisplacementsFromSensors {
    typedef MeshNatural<typename target_mesh_type::node_type> source_mesh_type;
    std::unique_ptr<source_mesh_type>   source_mesh_;
    std::unique_ptr<target_mesh_type>   target_mesh_;
    std::unique_ptr<interpolator_type> interpolator_;
  public:
    /**
     * \brief   Constructor for non-interpolated mesh.
     * \param   sensors_begin   iterator pointing to the beginning of the sensors we'll be using
     * \param   sensors_end     iterator pointing to the end of the sensors we'll be using
     *
     * With this constructor, a non-interpolated mesh will be used; sensor readings will be
     * provided as-is, in the original mesh corresponding to the physical layout of the 
     * skin sensors.
     */
    template <typename I=interpolator_type, EnableIf<std::is_same<I, none_placeholder> >... >
    DisplacementsFromSensors(
      SkinSensorIterator sensors_begin,
      SkinSensorIterator sensors_end
    )
    {
      // TODO build MeshNatural
    }


    /**
     * \brief   Constructor for interpolated mesh.
     *
     * This constructor will be used if we need to interpolate the sensory readings. We have
     * a mesh determined by the physical skin sensors layout and then we have another mesh
     * with "virtual" readigs which come from interpolation by some specific metehod.
     */
    template <typename I=interpolator_type, DisableIf<std::is_same<I, none_placeholder> >... >
    DisplacementsFromSensors(
      SkinSensorIterator sensors_begin,
      SkinSensorIterator sensors_end,
      std::unique_ptr<target_mesh_type> target_mesh,
      std::unique_ptr<interpolator_type> interpolator
    ) : target_mesh_(std::move(target_mesh)), interpolator_(std::move(interpolator))
    {
      // FIXME  check if this static_assert can be rewritten to be more general.
      //        I don't forsee any other MeshNode types, but if they become necessary,
      //        it'll be necessary to add them to this static_assert.
      static_assert(
           !std::is_same<target_mesh_type, MeshNatural<MeshNodeVal1d> >::value
        && !std::is_same<target_mesh_type, MeshNatural<MeshNodeVal3d> >::value,
        "You cannot use the mesh-interpolation constructor on a MeshNatural!\n"
        "NOTE: It doesn't make sense, because there's no interpolation for a natural mesh."
      );
    }
};


#endif /* DISPLACEMENTSFROMSENSORS_HPP */
