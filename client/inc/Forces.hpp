#ifndef FORCES_HPP
#define FORCES_HPP

#include <stdexcept>
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
  public:
    typedef typename mesh_type::values_container             values_container;
    /**
     * \brief   Default constructor
     * \param   mesh  std::unique_ptr to the mesh. This class takes ownership of
     *                the mesh. This param has to be std::move'd into here if
     *                you're constructing from an lvalue (a "non-temp").
     */
    Forces(std::unique_ptr<mesh_type> mesh)
      : mesh_((mesh) ? std::move(mesh) : throw std::runtime_error("Forces: Passed empty mesh unique_ptr"))
    {}
    const values_container& getRawValues() const { return mesh_->getRawValues(); }
    values_container& getRawValues()       { return mesh_->getRawValues(); }

};


#endif /* FORCES_HPP */
