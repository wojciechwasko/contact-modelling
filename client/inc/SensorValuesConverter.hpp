#ifndef SENSORVALUESCONVERTER_HPP
#define SENSORVALUESCONVERTER_HPP

#include <cstdint>
#include <cmath>

/**
 * \brief Converts raw values from SkinWare to real values in [m]
 * \param   v   Input raw value
 * \return  Scaled value in meters
 *
 * You might be wondering why we don't take v by const reference. Well, long story short it most
 * probably would not have been efficient. (-> Scott Meyers, Effective C++)
 */
double SensorValuesConverter(uint16_t v)
{
  // copy-paste from Luca's code, lulz
  const double d1 = 0.002;
  const double e0 = 8.85 * pow(10, -12);
  const double er = 4.37;  //dielettrico ecoflex 
  const double r_tax = 0.002;      //raggio taxel
  const double area = pow(r_tax, 2) * M_PI;     
  const double C1 = (e0 * er * area) / d1;  
  return d1 - double(e0 * er * area) / double(v * 0.32 * pow(10, -15) + C1);
}


#endif /* SENSORVALUESCONVERTER_HPP */
