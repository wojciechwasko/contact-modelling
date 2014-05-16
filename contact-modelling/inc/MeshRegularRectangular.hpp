#ifndef MESHREGULARRECTANGULAR_HPP
#define MESHREGULARRECTANGULAR_HPP

#include <cstdint>

#include "MeshInterface.hpp"

/**
 * \brief   Regular mesh with rectangular base element.
 *
 * \cond DEV
 * Later on, it'd be nice to implement custom iterators instead
 * of having to populate some internal std::vector. After all,
 * (x,y) positions are very easy to calculate on-the-fly.
 *
 * But would that be efficient? (POIROAE)
 * \endcond
 */
class MeshRegularRectangular : public MeshInterface
{
public:
  /**
   * \brief   (Almost) trivial constructor, empty mesh with no nodes.
   * \param   dim   dimensionality of values in this mesh
   */
  MeshRegularRectangular(const size_t dim, const double dx, const double dy);

  /**
   * \brief   Constructor for a regular rectangular mesh spanning a mesh of sensors -- from a "Natural
   *          Mesh" instance.
   * \param   base_mesh Any mesh supporting .min/maxX/Y(), but that's only natural meshes so far
   * \param   dx        Delta x, distance in x direction between two neighbouring nodes
   * \param   dy        Delta y, distance in y direction between two neighbouring nodes
   *
   * \cond DEV
   * This is merely a delegating constructor.
   * \endcond
   */
  MeshRegularRectangular(const MeshInterface& base_mesh, const double dx, const double dy);

  /**
   * \brief   Similar to above, but changes the dimensionality of values to dim
   */
  MeshRegularRectangular(const MeshInterface& base_mesh, const double dx, const double dy, const size_t dim);
  
  /**
   * \brief   Constructor for a regular square mesh going from x0 to x1 and from y0 to y1
   * \param   dim   dimensionality of values on this mesh
   * \param   x0    origin of the mesh (x direction)
   * \param   y0    origin of the mesh (y direction)
   * \param   x1    end of the mesh (x direction)
   * \param   y1    end of the mesh (y direction)
   * \param   dx    delta, distance in x direction between two neighbouring nodes
   * \param   dy    delta, distance in y direction between two neighbouring nodes
   *
   * The constructed mesh is guaranteed to contain the (x0,y0) and (x1,y1) points. In extreme
   * case, both of them might be on the boundaries of the mesh (if (x1-x0)/d or (y1-y0)/d is a
   * natural number). Otherwise, the rectangular mesh will be placed symmetrically, i.e. it will
   * extend the same distance "before" x0/y0 and "after" x1/y1.
   */
  MeshRegularRectangular(
    const size_t dim,
    const double x0,
    const double y0,
    const double x1,
    const double y1,
    const double dx,
    const double dy
  );
  
  MeshRegularRectangular& operator=(const MeshRegularRectangular&) = default;
  MeshRegularRectangular(const MeshRegularRectangular&)            = default;
  MeshRegularRectangular& operator=(MeshRegularRectangular&&)      = default;
  MeshRegularRectangular(MeshRegularRectangular&&)                 = default;
  ~MeshRegularRectangular()                                        = default;

private:
  /**
   * \brief   "delta", distance between two neighbouring nodes in x direction
   */
  double dx_;
  /**
   * \brief   "delta", distance between two neighbouring nodes in x direction
   */
  double dy_;

  double impl_node_area(size_t i) const;

  /**
   * \brief   How many nodes in total. The mesh is supposed to contain the x0,x1,y0,y1 points
   */
  static size_t calculate_no_nodes(double x0, double y0, double x1, double y1, double dx, double dy);

  /**
   * \brief   ceil((t1-t0)/d) ; no of nodes that wrap t0 and t1, with d distance between each pair
   */
  static size_t calculate_no_nodes_1D(double t0, double t1, double d);

  double impl_dx() const;
  double impl_dy() const;
};
#endif /* MESHREGULARRECTANGULAR_HPP */
