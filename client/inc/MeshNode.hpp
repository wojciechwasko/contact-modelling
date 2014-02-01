#ifndef MESHNODE_HPP
#define MESHNODE_HPP

#include <cstddef>

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with 1D values (e.g. only normal forces).
 */
template <std::size_t D>
struct MeshNode {
  const static size_t val_dimensionality = D;
  double x;
  double y;
  std::array<double*, D> vals;
};


#endif /* MESHNODE_HPP */
