#ifndef DISPLACEMENTSFROMSENSORSNATURAL_HPP
#define DISPLACEMENTSFROMSENSORSNATURAL_HPP

#include <memory>
#include <type_traits>
#include "traits_helpers.hpp"

#include "MeshInterface.hpp"
#include "MeshNatural.hpp"
#include "MeshNode.hpp"

/**
 * \brief   A class to represent displacements (deflections?) read from the senors
 */
template <class SkinConnector>
class DisplacementsFromSensorsNatural {
  public:
    typedef MeshNatural<MeshNode<1>, SkinConnector> mesh_type;
    typedef typename mesh_type::vals_vec_type  vals_vec_type;
    typedef typename mesh_type::node_type      node_type;

    /**
     * \brief   Constructor for non-interpolated mesh.
     * \param   sensors_begin   iterator pointing to the beginning of the sensors we'll be using
     * \param   sensors_end     iterator pointing to the end of the sensors we'll be using
     *
     * With this constructor, a non-interpolated mesh will be used; sensor readings will be
     * provided as-is, in the original mesh corresponding to the physical layout of the 
     * skin sensors.
     */
    DisplacementsFromSensorsNatural(
      SkinConnector& skin_conn
    ) :
      mesh_(new mesh_type(skin_conn.sensors_begin(), skin_conn.sensors_end())),
      skin_conn_(&skin_conn)
    {
    }

    /**
     * \brief   Get a constant reference to the values vector.
     * \note    In the interpolated case, this corresponds to the interpolated
     *          values.
     */
    const vals_vec_type& getVals() const { return mesh_->getVals(); }

    /**
     * \brief   Get a reference to the values vector.
     * \note    In the interpolated case, this corresponds to the interpolated
     *          values.
     */
    vals_vec_type& getVals() { return mesh_->getVals(); };

    const typename mesh_type::const_iterator
    nodes_cbegin() const { return mesh_->cbegin(); }

    const typename mesh_type::const_iterator
    nodes_cend() const { return mesh_->cend(); }

    typename mesh_type::iterator
    nodes_begin()  { return mesh_->begin(); }

    typename mesh_type::iterator
    nodes_end()  { return mesh_->end(); }

    const mesh_type& getMesh() const { return *mesh_; }

    void update() { skin_conn_->update(mesh_->getValues()); }

  private:
    std::unique_ptr<mesh_type> mesh_;
    SkinConnector* skin_conn_;
};

#endif /* DISPLACEMENTSFROMSENSORSNATURAL_HPP */
