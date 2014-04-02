#ifndef RESULTANTDISPLACEMENTS_HPP
#define RESULTANTDISPLACEMENTS_HPP

#include <stdexcept>
#include <memory>

template <class mesh_type>
class ResultantDisplacements {
    std::unique_ptr<mesh_type> mesh_;
    typedef typename mesh_type::values_container values_container;
  public:
    /**
     * \brief   Default constructor
     * \param   mesh  std::unique_ptr to the mesh. This class takes ownership of
     *                the mesh. This param has to be std::move'd into here if
     *                you're constructing from an lvalue (a "non-temp").
     */
    ResultantDisplacements(std::unique_ptr<mesh_type> mesh)
      : mesh_((mesh) ? std::move(mesh) : throw std::runtime_error("ResultantDisplacements: passed empty mesh unique_ptr"))
    {}
    const values_container& getRawValues() const { return mesh_->getRawValues(); }
    values_container& getRawValues()       { return mesh_->getRawValues(); }

};

#endif /* RESULTANTDISPLACEMENTS_HPP */
