#include "reconstruction.hpp"


std::istream& operator>>(std::istream& in, TractionType& traction_type)
{
  std::string token;
  in >> token;
  if (token == "pressures") {
    traction_type = TractionType::pressures;
  } else if (token == "forces") {
    traction_type = TractionType::forces;
  } else {
    throw boost::program_options::invalid_option_value(token);
  }
  return in;
}

std::istream& operator>>(std::istream& in, InputType& input_type)
{
  std::string token;
  in >> token;
  if (token == "yaml") {
    input_type = InputType::yaml;
  } else if (token == "luca") {
    input_type = InputType::luca;
  } else {
    throw boost::program_options::invalid_option_value(token);
  }
  return in;
}

std::istream& operator>>(std::istream& in, cm::NIPP& nipp_value)
{
  std::string token;
  in >> token;
  if (token == "InterpolateToZero") {
    nipp_value = cm::NIPP::InterpolateToZero;
  } else if (token == "RemoveFromGrid") {
    nipp_value = cm::NIPP::RemoveFromGrid;
  } else {
    throw boost::program_options::invalid_option_value(token);
  }
  return in;
}
