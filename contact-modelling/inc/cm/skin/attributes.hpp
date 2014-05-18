#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

/**
 * \file
 * \brief   Definition of SkinAttributes struct
 */

namespace cm {

/**
 * \brief A struct with basic informations about the skin
 */
struct SkinAttributes {
  /**
   * Thickness of the elastomer layer.
   */
  double h;

  /**
   * Young modulus of the elastomer.
   */
  double E;

  /**
   * Poisson's ratio of the elastomer.
   */
  double nu;

  /**
   * The radius of one given taxel (Taxels are assumed to be circular).
   */
  double taxelRadius;
};

} /* namespace cm */

#endif /* ATTRIBUTES_HPP */
