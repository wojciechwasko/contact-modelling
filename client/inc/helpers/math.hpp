#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

namespace helpers {
  namespace math {
    /**
     * \brief   Returns |v1-v2| < eps
     */
    bool eq_almost(double v1, double v2, double eps) {
      return std::abs(v1-v2) < eps;
    }
  }
}


#endif /* MATH_HPP */
