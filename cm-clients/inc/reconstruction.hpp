#ifndef RECONSTRUCTION_HPP
#define RECONSTRUCTION_HPP

#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "AlgForcesToDisplacements.hpp"
#include "AlgDisplacementsToForces.hpp"

struct opts {
  double source_pitch;
  double forces_pitch;
  size_t forces_dim;
  double displacements_pitch;
  size_t displacements_dim;
  std::string input_filename;
};

bool process_options(int argc, char** argv, opts& options);


#endif /* RECONSTRUCTION_HPP */
