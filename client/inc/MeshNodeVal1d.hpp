#ifndef MESHNODEVAL1D_HPP
#define MESHNODEVAL1D_HPP

#include <cstddef>

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with 1D values (e.g. only normal forces).
 */
typedef struct MeshNodeVal1d {
  const static size_t val_dimensionality = 1;
  double x;
  double y;
  std::array<double*,1> vals;
} MeshNodeVal1d;


#endif /* MESHNODEVAL1D_HPP */
