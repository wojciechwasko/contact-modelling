#ifndef RECONSTRUCTION_HPP
#define RECONSTRUCTION_HPP

#include <iostream>
#include <string>
#include <memory>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

#include "cm/cm.hpp"

enum class TractionType { pressures, forces };
std::istream& operator>>(std::istream& in, TractionType& traction_type);

enum class InputType { yaml, luca };
std::istream& operator>>(std::istream& in, InputType& input_type);

std::istream& operator>>(std::istream& in, cm::NIPP& nipp_value);

struct options_type {
  TractionType  traction_type;
  InputType     input_type;
  cm::NIPP      interpolator_policy;
  double source_pitch;
  double tractions_pitch;
  bool nonnegative_tractions;
  double reconstructed_pitch;
  std::string input;
};

struct suite_type {
  std::unique_ptr<cm::SkinProviderInterface> skin_provider;
  std::unique_ptr<cm::Grid>         raw_grid;
  std::unique_ptr<cm::Grid>         interp_grid;
  std::unique_ptr<cm::InterpolatorInterface> interpolator;
  std::unique_ptr<cm::Grid>         tractions_grid;
  std::unique_ptr<cm::Grid>         reconstructed_grid;
  std::unique_ptr<cm::AlgInterface> to_tractions;
  boost::any                        to_tractions_params;
  boost::any                        to_tractions_precomputed;
  std::unique_ptr<cm::AlgInterface> to_reconstructed;
  boost::any                        to_reconstructed_params;
  boost::any                        to_reconstructed_precomputed;
};

options_type process_options(int argc, char** argv);
suite_type construct_suite(const options_type& options);

#endif /* RECONSTRUCTION_HPP */
