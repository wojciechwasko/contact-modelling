#ifndef MESHINTERFACE_HPP
#define MESHINTERFACE_HPP

#include <cstdint>
#include <vector>
#include <boost/any.hpp>

#include "cm/mesh/node.hpp"

namespace cm {

/**
 * \brief   A base class to provide a common interface and various helper functions
 *
 */
class MeshInterface {
public:
  const size_t D;

  typedef MeshNode                                   node_type;
  typedef std::vector<node_type>                     nodes_container;
  typedef typename nodes_container::iterator         nodes_iterator;
  typedef typename nodes_container::const_iterator   nodes_const_iterator;
  typedef double                                     value_type;
  typedef std::vector<value_type>                    values_container;
  typedef boost::any                                 metadata_type;
  typedef std::vector<boost::any>                    metadata_container;
  typedef std::vector<size_t>                        bad_nodes_type;

  /**
   * \brief   Copy the nodes over from clone_from mesh.
   *
   * Removes all the nodes (and values and metadata) from this mesh and adds nodes with the
   * coordinates of the nodes from the clone_from mesh.
   */
  void clone_structure(const MeshInterface& clone_from);

  /**
   * \brief   Get number of nodes in the mesh.
   */
  size_t no_nodes() const;
  /**
   * \brief   Get number of nodes in the mesh.
   */
  size_t size() const;

  /**
   * \brief   Get non-const iterator to the bein of nodes.
   */
  nodes_iterator nodes_begin();

  /**
   * \brief   Get const iterator to the begin of nodes.
   */
  nodes_const_iterator nodes_cbegin() const;

  /**
   * \brief   Get non-const iterator to the end of nodes.
   */
  nodes_iterator nodes_end();

  /**
   * \brief   Get const iterator to the end of nodes.
   */
  nodes_const_iterator nodes_cend() const;

  /**
   * \brief   Get a reference to the node at position i, const version.
   */
  const node_type& node(size_t i) const;

  /** 
   * \brief   Return the area a node is "responsible" for.
   *
   * For MeshNatural, this would be the area of Voronoi cell, for RegularSquare, dx*dy etc.
   */
  double node_area(size_t i) const;

  /**
   * \brief   Get a reference to the node at position i, non-const version.
   */
  node_type& node(size_t i);

  /**
   * \brief   Get bulk access to the values vector (const version)
   *
   */
  const values_container& getRawValues() const;

  /**
   * \brief   Get bulk access to the values vector (non-const version)
   *
   */
  values_container& getRawValues();

  /**
   * \brief   Return a local copy of values for node i
   * \param   i   ID (number of the node)
   * \note    This is not very efficient, since we're making a local copy and returning it.
   *
   * In future, if needed, it might be implement something resembling Eigen's ::block()
   * functionality to allow the returned types to be used as lvalues.
   */
  std::vector<value_type> getValues(size_t i) const;

  /**
   * \brief   Get vi-th value for i-th node.
   * \param   i   ID (number) of the node
   * \param   vi  which value from the node to get
   */
  value_type getValue(size_t i, size_t vi) const;

  /**
   * \brief   Set vi-th value for i-th node.
   * \param   i   ID (number) of the node
   * \param   vi  which value from the node to get
   * \param   v   value to be set
   */
  void setValue(size_t i, size_t vi, value_type v);

  /**
   * \brief   Get a const reference to the metadata of a node at position i.
   */
  const metadata_type& getMetadata(const size_t i) const;

  /**
   * \brief   Set the metadata of a node at position i.
   */
  void setMetadata(const size_t i, const metadata_type& rhs);

  /**
   * \brief   Set the information on what points are bad.
   */
  void setBadNodes(const bad_nodes_type& bad_points);

  const bad_nodes_type& getBadNodes() const;
        bad_nodes_type& getBadNodes();

  /**
   * \brief   Remove points (and their associated values and metadata) from the mesh
   * \param   indices  A sorted vector of unique IDs of points to be erased
   */
  void erase(const std::vector<size_t>& indices);

  double minX() const;
  double minY() const;
  double maxX() const;
  double maxY() const;

  /**
   * \brief   Hackety hack! :D Return dx (spacing in x on the mesh and the length of x side on a
   *                        cuboid representing value in a node)
   *
   * This method is virtual which means that if the deriving class overrides it, the derived
   * implementation will be called. Otherwise the default will be called.
   *
   * The hack is in the fact that we take the base elements to be square of the area of the first
   * node, i.e. the returned value will be sqrt(node_area(0)). While this is a gross generalisation
   * and may not be accurate, this will do for what we want to achieve.
   *
   * If it later turns out we will *actually* be using meshes with variable node areas, it might
   * still be useful to represent them as cuboids, but with different x,y sides lengths. In that
   * case, matplotlib's dx,dy can be an array as well, meaning that this could be done.
   *
   * \note  If the base implementation of this method is called, it will log a WARN about the
   *        simplification described above.
   */
  double dx() const;

  /**
   * \brief   Hackety hack! :D Return dy (spacing in y on the mesh and the length of y side on a
   *                        cuboid representing value in a node)
   *
   * This method is virtual which means that if the deriving class overrides it, the derived
   * implementation will be called. Otherwise the default will be called.
   *
   * The hack is in the fact that we take the base elements to be square of the area of the first
   * node, i.e. the returned value will be sqrt(node_area(0)). While this is a gross generalisation
   * and may not be accurate, this will do for what we want to achieve.
   *
   * If it later turns out we will *actually* be using meshes with variable node areas, it might
   * still be useful to represent them as cuboids, but with different x,y sides lengths. In that
   * case, matplotlib's dx,dy can be an array as well, meaning that this could be done.
   *
   * \note  If the base implementation of this method is called, it will log a WARN about the
   *        simplification described above.
   */
  double dy() const;
protected:
  void generateMinMax();

  /**
   * \brief   Constructor.
   * \param   dim       Dimensionality of the mesh.
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
  MeshInterface(const size_t dim, const size_t no_nodes);

  MeshInterface& operator=(const MeshInterface&) = default;
  MeshInterface(const MeshInterface&)            = default;
  MeshInterface& operator=(MeshInterface&&)      = default;
  MeshInterface(MeshInterface&&)                 = default;

public:
  virtual ~MeshInterface()                       = default;

private:

  double min_x_;    double min_y_;
  double max_x_;    double max_y_;

  /**
   * \brief   Values in the mesh'es nodes. 
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
  metadata_container  metadata_;

  bad_nodes_type      bad_nodes_;  

  /**
   * \brief   Implementation of the node area function
   */
  virtual double impl_node_area(size_t i) const = 0;
  virtual double impl_dx() const;
  virtual double impl_dy() const;
};

} /* namespace cm */

#endif /* MESHINTERFACE_HPP */
