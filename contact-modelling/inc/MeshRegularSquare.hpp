#ifndef MESHREGULARSQUARE_HPP
#define MESHREGULARSQUARE_HPP

#include <cstdint>

#include "MeshRegularRectangular.hpp"

/**
 * \brief   Regular mesh with square base element.
 *
 * \cond DEV
 * Later on, it'd be nice to implement custom iterators instead
 * of having to populate some internal std::vector. After all,
 * (x,y) positions are very easy to calculate on-the-fly.
 *
 * But would that be efficient? (POIROAE)
 * \endcond
 */
class MeshRegularSquare : public MeshRegularRectangular
{
public:
  /**
   * \brief   (Almost) trivial constructor, empty mesh, no nodes.
   * \param   dim   dimensionality of values in this mesh
   * \param   d     dimension of the base element in this mesh
   */
  MeshRegularSquare(const size_t dim, const double d);

  /**
   * \brief   Constructor for a regular square mesh spanning a mesh of sensors -- from a "Natural
   *          Mesh" instance.
   * \param   base_mesh Any mesh supporting .min/maxX/Y(), but that's only natural meshes so far
   * \param   d         Delta, distance in x/y direction between two neighbouring nodes
   *
   * \cond DEV
   * This is merely a delegating constructor.
   * \endcond
   */
  MeshRegularSquare(const MeshInterface& base_mesh, const double d);

  /**
   * \brief   Similar to above, but changes the dimensionality of values to dim
   */
  MeshRegularSquare(const MeshInterface& base_mesh, const double d, const size_t dim);
  
  /**
   * \brief   Constructor for a regular square mesh going from x0 to x1 and from y0 to y1
   * \param   dim   dimensionality of values on this mesh
   * \param   x0    origin of the mesh (x direction)
   * \param   y0    origin of the mesh (y direction)
   * \param   x1    end of the mesh (x direction)
   * \param   y1    end of the mesh (y direction)
   * \param   d     delta, distance in x/y direction between two neighbouring nodes
   *
   * The constructed mesh is guaranteed to contain the (x0,y0) and (x1,y1) points. In extreme
   * case, both of them might be on the boundaries of the mesh (if (x1-x0)/d or (y1-y0)/d is a
   * natural number). Otherwise, the rectangular mesh will be placed symmetrically, i.e. it will
   * extend the same distance "before" x0/y0 and "after" x1/y1.
   */
  MeshRegularSquare(
    const size_t dim,
    const double x0,
    const double y0,
    const double x1,
    const double y1,
    const double d
  );
  
  MeshRegularSquare& operator=(const MeshRegularSquare&) = default;
  MeshRegularSquare(const MeshRegularSquare&)            = default;
  MeshRegularSquare& operator=(MeshRegularSquare&&)      = default;
  MeshRegularSquare(MeshRegularSquare&&)                 = default;
  ~MeshRegularSquare()                                   = default;

};
#endif /* MESHREGULARSQUARE_HPP */
