#ifndef SENSORVALUESCONVERTER_HPP
#define SENSORVALUESCONVERTER_HPP

#include <cstdint>

/**
 * \brief Converts raw values from SkinWare to real values in [m]
 * \param   v   Input raw value
 * \return  Scaled value in meters
 *
 * You might be wondering why we don't take v by const reference. Well, long story short it most
 * probably would not have been efficient. (-> Scott Meyers, Effective C++)
 */
double SensorValuesConverter(uint16_t v);


#endif /* SENSORVALUESCONVERTER_HPP */
