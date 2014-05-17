#ifndef MESHREGULARSQUARE_HPP
#define MESHREGULARSQUARE_HPP

#include <cstdint>

#include "cm/mesh/rectangular_base.hpp"

namespace cm {
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
class MeshSquareBase : public MeshRectangularBase
{
public:
  /**
   * \brief   (Almost) trivial constructor, empty mesh, no nodes.
   * \param   dim   dimensionality of values in this mesh
   * \param   d     dimension of the base element in this mesh
   */
  MeshSquareBase(const size_t dim, const double d);

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
  MeshSquareBase(const MeshInterface& base_mesh, const double d);

  /**
   * \brief   Similar to above, but changes the dimensionality of values to dim
   */
  MeshSquareBase(const MeshInterface& base_mesh, const double d, const size_t dim);
  
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
  MeshSquareBase(
    const size_t dim,
    const double x0,
    const double y0,
    const double x1,
    const double y1,
    const double d
  );
  
  MeshSquareBase& operator=(const MeshSquareBase&) = default;
  MeshSquareBase(const MeshSquareBase&)            = default;
  MeshSquareBase& operator=(MeshSquareBase&&)      = default;
  MeshSquareBase(MeshSquareBase&&)                 = default;
  ~MeshSquareBase()                                   = default;

};

} /* namespace cm */

#endif /* MESHREGULARSQUARE_HPP */
