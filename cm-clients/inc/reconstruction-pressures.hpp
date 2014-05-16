#ifndef RECONSTRUCTION_PRESSURES_HPP
#define RECONSTRUCTION_PRESSURES_HPP

#include <string>
#include <cstddef>

struct opts {
  double source_pitch;
  double pressures_pitch;
  double displacements_pitch;
  std::string input_filename;
};

bool process_options(int argc, char** argv, opts& options);


#endif /* RECONSTRUCTION_PRESSURES_HPP */
