#ifndef FORCES_HPP
#define FORCES_HPP

#include <memory>

#include "MeshInterface.hpp"

/**
 * \brief   A helper class to represent distribution of forces over a mesh
 * \tparam  mesh_type   type of the mesh we suppose the values are distributed
 *                      over. Note that this also defines whether the forces are
 *                      1D or 3D, as the mesh defines that.
 */
template <class mesh_type>
class Forces {
    std::unique_ptr<mesh_type> mesh_;
    typedef typename mesh_type::container_reference             container_reference;
    typedef typename mesh_type::container_const_reference container_const_reference;
  public:
    /**
     * \brief   Default constructor
     * \param   mesh  std::unique_ptr to the mesh. This class takes ownership of
     *                the mesh. This param has to be std::move'd into here if
     *                you're constructing from an lvalue (a "non-temp").
     */
    Forces(std::unique_ptr<mesh_type> mesh) : mesh_(std::move(mesh)) {}
          container_reference getRawValues() const { return mesh_->getRawValues(); }
    container_const_reference getRawValues()       { return mesh_->getRawValues(); }

};


#endif /* FORCES_HPP */
