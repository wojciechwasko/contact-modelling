#ifndef DISPLACEMENTSFROMSENSORS_HPP
#define DISPLACEMENTSFROMSENSORS_HPP

#include <type_traits>

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
 * \tparam  targetMeshType      Optional. Type of mesh the values will be interpolated onto. By default
 *                              the sensors' values (after scaling) will be offered as-is, in the
 *                              "natural" mesh. It is however possible to interpolate them within this
 *                              class by using a custom interpolation method.
 * \tparam  interpolator        Optional. Algorithm used to interpolate values from one mesh onto
 *                              another. Used only if targetMeshType != MeshNormal
 */
template <
  class SkinSensorIterator,
  class targetMeshType = MeshNatural<MeshNodeVal1d>,
  class interpolator = void
>
class DisplacementsFromSensors {
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
    DisplacementsFromSensors(
      SkinSensorIterator sensors_begin,
      SkinSensorIterator sensors_end,
      const MeshInterface<targetMeshType> & mesh,
      const InterpolatorInterface<Interpolator, MeshNatural<MeshNodeVal1d>, targetMeshType> & interpolator
    )
    {
      // FIXME  check if this static_assert can be rewritten to be more general.
      //        I don't forsee any other MeshNode types, but if they become necessary,
      //        it'll be necessary to add them to this static_assert.
      static_assert(
           !std::is_same<targetMeshType, MeshNatural<MeshNodeVal1d> >::value
        && !std::is_same<targetMeshType, MeshNatural<MeshNodeVal3d> >::value,
        "You cannot use the mesh-interpolation constructor on a MeshNatural!\n"
        "NOTE: It doesn't make sense, because there's no interpolation for a natural mesh."
      );
    }
};


#endif /* DISPLACEMENTSFROMSENSORS_HPP */
