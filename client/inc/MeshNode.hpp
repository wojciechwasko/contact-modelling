#ifndef MESHNODE_HPP
#define MESHNODE_HPP

#include <cstddef>

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with
 *          D values.
 * \tparam  D   dimensionality of the values stored in the node.
 * \note    The values are not stored here directly; rather, a pointer to the
 *          value will be stored; the values are stored on Mesh'es side (usually 
 *          in a vector/array which will ensure continuity of memory).
 *
 * As an example, let's consider a 3D value (e.g. 3D forces)
 * vals[0] - Fx
 * vals[1] - Fy
 * vals[2] - Fz
 */
template <std::size_t D>
struct MeshNode {
  const static size_t val_dimensionality = D;
  double x;
  double y;
  std::array<double*, D> vals;
};


#endif /* MESHNODE_HPP */
