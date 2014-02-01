#ifndef MESHNODEVAL3D_HPP
#define MESHNODEVAL3D_HPP

#include <array>
#include <cstddef>

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with 3D values (e.g. forces with all the components).
 */
typedef struct MeshNodeVal3d {
  const static size_t val_dimensionality = 3;
  double x;
  double y;
  std::array<double*, 3> vals;
} MeshNodeVal3d;


#endif /* MESHNODEVAL3D_HPP */
