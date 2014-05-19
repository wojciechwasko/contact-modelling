#ifndef SKINPROVIDERINTERFACE_HPP
#define SKINPROVIDERINTERFACE_HPP

#include <vector>
#include <cstdint>
#include <functional>

#include "cm/skin/attributes.hpp"

/**
 * \file
 * \brief   Definition of interface for skin providers
 */

namespace cm {

class Grid;

/**
 * \brief   A class which will provide the skin (location of taxels and their response values).
 */
class SkinProviderInterface {
public:
  typedef std::vector<double> target_values_type;

  /**
   * Sensor response value dimensionality
   */
  const size_t D;
  
  /**
   * \brief   Return a new Grid.
   * \param   dim   dimensionality of the Netural Grid'es values
   * \return  a *bare* pointer to a newly constructed natural Grid
   *
   * \note  This newly created grid does *not* have its values initialised.
   */
  Grid* createGrid() const;

  /**
   * \brief   Update values.
   *
   * New values are requested by the provider and than inserted into the
   * target_vec vector.
   */
  void update(target_values_type& target_vec) const;

  /**
   * \brief   Return information about physical aspects of the skin
   */
  SkinAttributes getAttributes() const;

protected:
  SkinProviderInterface(const size_t dim);

  SkinProviderInterface& operator=(const SkinProviderInterface&)  = default;
  SkinProviderInterface(const SkinProviderInterface&)             = default;
  SkinProviderInterface& operator=(SkinProviderInterface&&)       = default;
  SkinProviderInterface(SkinProviderInterface&&)                  = default;

public:
  virtual ~SkinProviderInterface()                                = default;

private:
  /**
   * \brief   To be overriden by implementation
   */
  virtual Grid* impl_createGrid() const = 0;
  /**
   * \brief   To be overriden by implementation
   */
  virtual void impl_update(target_values_type& target_vec) const = 0;
  /**
   * \brief   To be overriden by implementation
   */
  virtual SkinAttributes impl_getAttributes() const = 0;
};

} /* namespace cm */


#endif /* SKINPROVIDERINTERFACE_HPP */
