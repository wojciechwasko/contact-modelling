#ifndef MESHNODE_HPP
#define MESHNODE_HPP

#include <cstddef>

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with
 *          dim values.
 * \tparam  dim   dimensionality of the values stored in the node.
 */
template <std::size_t dim>
struct MeshNode {
  constexpr static size_t D = dim;
  double x;
  double y;
};


#endif /* MESHNODE_HPP */
