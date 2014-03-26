#include <cstdlib>
#include "SensorValuesConverter.hpp"

double SensorValuesConverter(uint16_t v)
{
  // TODO actually implement this.
  return 10 * ((double) std::rand()) / RAND_MAX;
}
