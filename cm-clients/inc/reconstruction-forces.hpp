#ifndef RECONSTRUCTION_FORCES_HPP
#define RECONSTRUCTION_FORCES_HPP

#include <string>
#include <cstddef>

struct opts {
  double source_pitch;
  double forces_pitch;
  size_t forces_dim;
  double displacements_pitch;
  size_t displacements_dim;
  std::string input_filename;
};

bool process_options(int argc, char** argv, opts& options);


#endif /* RECONSTRUCTION_FORCES_HPP */
