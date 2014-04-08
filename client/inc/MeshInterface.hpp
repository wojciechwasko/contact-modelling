#ifndef MESHINTERFACE_HPP
#define MESHINTERFACE_HPP

#include <array>
#include <cstddef>

#include <boost/any.hpp>

#include "MeshNode.hpp"
#include "helpers/container_algorithms.hpp"

/**
 * \def COMMA
 * A nifty hack to enable processing of multiple templated classes in macros.
 * Inspired by http://stackoverflow.com/a/19841470/312650
 */
#ifndef COMMA
#define COMMA ,
#endif

/**
 * Injects chosen typedefs defined in MeshInterface
 */
#define INJECT_MESH_INTERFACE_TYPES(name) \
  using typename MeshInterface< name >::node_type; \
  using typename MeshInterface< name >::nodes_container; \
  using typename MeshInterface< name >::nodes_iterator; \
  using typename MeshInterface< name >::nodes_const_iterator; \
  using typename MeshInterface< name >::value_type; \
  using typename MeshInterface< name >::values_container; \
  using typename MeshInterface< name >::metadata_type; \
  using typename MeshInterface< name >::metadata_container;

template <class derived_t>
struct MeshImpl_traits;

/**
 * \brief   A mixin class to provide a common interface and various helper functions
 *
 * While it doesn't seem crucial right now, it'll come in handy when
 * we have to implement functions like extractROI and such.
 *
 * As we add more functionality to this class, it might turn out that "MeshBase" would've been more
 * appropriate, but it's merely a name. Plus, "*Base" strongly indicates a vertical, classical
 * inheritance, not a mixin-based polymorphism.
 */
template <class Derived, size_t dim>
class MeshInterface {
  public:
    constexpr static size_t D = dim;

    typedef MeshNode                                   node_type;
    typedef std::vector<node_type>                     nodes_container;
    typedef typename nodes_container::iterator         nodes_iterator;
    typedef typename nodes_container::const_iterator   nodes_const_iterator;
    typedef double                                     value_type;
    typedef std::vector<value_type>                    values_container;
    typedef boost::any                                 metadata_type;
    typedef std::vector<boost::any>                    metadata_container;

    /**
     * \brief   Get number of nodes in the mesh.
     */
    size_t no_nodes() const { return nodes_.size(); }
    /**
     * \brief   Get number of nodes in the mesh.
     */
    size_t size() const { return no_nodes(); }


    /**
     * \brief   Get non-const iterator to the bein of nodes.
     */
    nodes_iterator nodes_begin() { return nodes_.begin(); }

    /**
     * \brief   Get const iterator to the begin of nodes.
     */
    nodes_const_iterator nodes_cbegin() const { return nodes_.cbegin(); }

    /**
     * \brief   Get non-const iterator to the end of nodes.
     */
    nodes_iterator nodes_end() { return nodes_.end(); }

    /**
     * \brief   Get const iterator to the end of nodes.
     */
    nodes_const_iterator nodes_cend() const { return nodes_.cend(); }

    /**
     * \brief   Get a reference to the node at position i, const version.
     * \note    This is not bound-checked.
     */
    const node_type& node(size_t i) const { return nodes_[i]; }

    /** 
     * \brief   Return the area a node is "responsible" for.
     *
     * For MeshNatural, this would be the area of Voronoi cell, for RegularSquare, dx*dy etc.
     */
    const double node_area(size_t i) const
    {
      return static_cast<const Derived *>(this)->impl_node_area(i);
    }

    /**
     * \brief   Get a reference to the node at position i, non-const version.
     * \note    This is not bound-checked.
     */
    node_type& node(size_t i) { return nodes_[i]; }

    /**
     * \brief   Get bulk access to the values vector (const version)
     *
     */
    const values_container& getRawValues() const
    {
      return values_;
    }

    /**
     * \brief   Get bulk access to the values vector (non-const version)
     *
     */
    values_container& getRawValues() 
    {
      return values_;
    }

    /**
     * \brief   Return a local copy of values for node i
     * \param   i   ID (number of the node)
     * \note    This is not bound-checked
     * \note    This is not very efficient, since we're making a local copy and returning it.
     *
     * In future, if needed, it might be implement something resembling Eigen's ::block()
     * functionality to allow the returned types to be used as lvalues.
     */
    std::array<value_type, D> getValues(size_t i) const
    {
      std::array<value_type, D> ret;
      for (size_t it = 0; it < D; ++it)
        ret[it] = values_[i*D + it];
      return ret;
    }

    /**
     * \brief   Get vi-th value for i-th node.
     * \param   i   ID (number) of the node
     * \param   vi  which value from the node to get
     * \note    This is not bound-checked.
     */
    value_type getValue(size_t i, size_t vi) const
    {
      return values_[i*D + vi];
    }

    /**
     * \brief   Set vi-th value for i-th node.
     * \param   i   ID (number) of the node
     * \param   vi  which value from the node to get
     * \param   v   value to be set
     * \note    This is not bound-checked.
     */
    void setValue(size_t i, size_t vi, value_type v)
    {
      values_[i*D + vi] = v;
    }

    /**
     * \brief   Get a const reference to the metadata of a node at position i.
     * \note    This is not bound-checked.
     */
    const metadata_type& getMetadata(const size_t i) const
    {
      return metadata_[i];
    }

    /**
     * \brief   Set the metadata of a node at position i.
     * \note    This is not bound-checked.
     */
    void setMetadata(const size_t i, const metadata_type& rhs) {
      metadata_[i] = rhs;
    };

    /**
     * \brief   Remove points (and their associated values and metadata) from the mesh
     * \param   indices  A sorted vector of unique IDs of points to be erased
     */
    void erase(const std::vector<size_t>& indices)
    {
      using helpers::container_algorithms::erase_by_indices;
      erase_by_indices(nodes_,      indices);
      erase_by_indices(metadata_,   indices);
      erase_by_indices(values_,     indices, D);
      static_cast<Derived*>(this)->hook_post_erase(indices);
    }

  protected:
    /**
     * \brief   Constructor.
     * \param   no_nodes  Number of nodes in the mesh.
     *
     * The number of nodes in the mesh is used for preallocation of memory.
     * \note    The memory (values inside vectors) is *NOT* initialized (in case of primitive types)
     *          or initialized by default constructors (in case of non-POD style data). Since by C++
     *          rules, a base class is guaranteed to be fully constructed by the time the derived
     *          class' constructor is called, the derived class' constructor can rely the values
     *          being there in the vectors, i.e. should not push_back (or whatever the interface
     *          will be called) new nodes.
     */
    MeshInterface(size_t no_nodes)
      : values_(D * no_nodes),
        nodes_(no_nodes),
        metadata_(no_nodes)
    {}

    MeshInterface& operator=(const MeshInterface&) = default;
    MeshInterface(const MeshInterface&)            = default;
    MeshInterface& operator=(MeshInterface&&)      = default;
    MeshInterface(MeshInterface&&)                 = default;
    ~MeshInterface()                               = default;

    /**
     * \brief   Default, empty hook called after erasing nodes from the mesh.
     * \param   indices   IDs of nodes which were erased
     *
     * If a child class will not override this method, this implementation will be called.
     */
    void hook_post_erase(const std::vector<size_t>& indices) { }

  private:
    /**
     * \brief   Values in the mesh'es nodes. 
     *
     * TODO   Think of enhancing encapsulation. The way I'm thinking to do it now is to 
     *        both write and read the values (vals) from Node stuct's pointers; hence,
     *        there's no granularity in access. A much better solution would allow the
     *        Interpolators/Algorithms to access the values rw, while the rest of the world,
     *        by ro. A solution somewhere in the middle would be to have additional ro pointers
     *        in the Node struct.
     */
    values_container           values_;

    /**
     * \brief   A vector of nodes in the mesh
     */
    nodes_container  nodes_;
    
    /**
     * \brief   A vector of metadata. 
     *
     * \note    We're using boost::any for sake of generality. At time of instantiation of the mesh,
     *          we don't know what is the type of the metadata to be stored here. This field is
     *          intended to be primarily used by interpolators, to store data such as the
     *          barycentric coordinates and IDs of the corresponding nodes constituting e.g. a
     *          Delaunay triangle in the source mesh.
     *
     * \note    Data will be removed from this vector if a node is removed from the mesh. On the
     *          other hand, if this is a target mesh and a node is removed from a source mesh and
     *          the IDs of nodes from the source mesh are stored inside metadata, this class has (in
     *          principle) no way of knowing this and will not update accordingly. That is to say
     *          that if you store IDs of nodes from some other mesh, this will not be updated if
     *          you remove nodes from the other mesh. I guess this behaviour is to be expected, but
     *          I felt it needed to be pointed out explicitly.
     */
    metadata_container metadata_;
};


#endif /* MESHINTERFACE_HPP */
