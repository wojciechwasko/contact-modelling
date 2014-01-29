#ifndef MESHNODEVAL1D_HPP
#define MESHNODEVAL1D_HPP

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with 1D values (e.g. only normal forces).
 */
typedef struct MeshNodeVal1d {
  double x;
  double y;
  double* const val;
} MeshNodeVal1d;


#endif /* MESHNODEVAL1D_HPP */
