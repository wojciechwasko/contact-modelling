#ifndef SKIN_HELPERS_HPP
#define SKIN_HELPERS_HPP

#include <cstddef>

/**
 * \cond DEV
 */

/**
 * \file
 * \brief   Functions to help interfaceing with SkinWare.
 */

namespace cm {
namespace details {

/**
 * \brief   Counts number of elements between it1 and it2
 * \param   it1   first iterator (forward!)
 * \param   it2   second iterator (forward!)
 *
 * Due to crude implementation of skin's iterators, we can't use std::distance
 * on them. This is a replacement
 *
 * \todo  Specialise std::distance for SkinWare's iterators!
 */
template <class I>
size_t distance(I it1, I it2) {
  size_t d = 0;
  for (I it = it1; it != it2; ++it) {
    ++d;
  }
  return d;
}

} /* namespace details */
} /* namespace cm */

/**
 * \endcond
 */

#endif /* SKIN_HELPERS_HPP */
