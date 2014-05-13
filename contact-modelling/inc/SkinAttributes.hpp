#ifndef SKINATTRIBUTES_HPP
#define SKINATTRIBUTES_HPP

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


#endif /* SKINATTRIBUTES_HPP */
