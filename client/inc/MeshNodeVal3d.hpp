#ifndef MESHNODEVAL3D_HPP
#define MESHNODEVAL3D_HPP

/**
 * \brief   A "POD" struct to keep x,y coordinates of a node in the mesh; with 3D values (e.g. forces with all the components).
 */
typedef struct MeshNodeVal3d {
  double x;
  double y;
  double* const val_x;
  double* const val_y;
  double* const val_z;
} MeshNodeVal3d;


#endif /* MESHNODEVAL3D_HPP */
