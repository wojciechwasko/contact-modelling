#ifndef ATTRIBUTES_HPP
#define ATTRIBUTES_HPP

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
   * The area of one given taxel.
   */
  double taxelArea;
};

} /* namespace cm */

#endif /* ATTRIBUTES_HPP */
