#ifndef RECONSTRUCTION_HPP
#define RECONSTRUCTION_HPP

#include "MeshNatural.hpp"
#include "MeshRegularSquare.hpp"
#include "InterpolatorLinearDelaunay.hpp"
#include "AlgPressuresToDisplacements.hpp"
#include "AlgDisplacementsToPressures.hpp"

struct opts {
  double source_pitch;
  double pressures_pitch;
  double displacements_pitch;
  std::string input_filename;
};

bool process_options(int argc, char** argv, opts& options);


#endif /* RECONSTRUCTION_HPP */
