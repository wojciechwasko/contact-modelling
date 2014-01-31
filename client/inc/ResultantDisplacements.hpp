#ifndef RESULTANTDISPLACEMENTS_HPP
#define RESULTANTDISPLACEMENTS_HPP

#include <memory>

template <class mesh_type>
class ResultantDisplacements {
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
    ResultantDisplacements(std::unique_ptr<mesh_type> mesh) : mesh_(std::move(mesh)) {}
          container_reference getRawValues() const { return mesh_->getRawValues(); }
    container_const_reference getRawValues()       { return mesh_->getRawValues(); }

};

#endif /* RESULTANTDISPLACEMENTS_HPP */
