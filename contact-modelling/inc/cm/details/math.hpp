#ifndef DETAILS_MATH_HPP
#define DETAILS_MATH_HPP

#include <cmath>

/**
 * \cond DEV
 */

/**
 * \file 
 * \brief   Mathematical helpers.
 */

namespace cm {
namespace details {

/**
 * \brief   Returns bool(|v1-v2| < eps)
 */
inline bool eq_almost(double v1, double v2, double eps) {
  return std::abs(v1-v2) < std::abs(eps);
}

} /* namespace details */
} /* namespace cm */

/**
 * \endcond
 */

#endif /* DETAILS_MATH_HPP */
