#ifndef SKIN_HELPERS_HPP
#define SKIN_HELPERS_HPP

#include <cstddef>

namespace skin_helpers {
  /**
   * \brief   Counts number of elements between it1 and it2
   * \param   it1   first iterator (forward!)
   * \param   it2   second iterator (forward!)
   *
   * Due to crude implementation of skin's iterators, we can't use std::distance
   * on them. This is a replacement
   */
  template <class it_t>
  size_t distance(it_t it1, it_t it2) {
    size_t d = 0;
    for (it_t it = it1; it != it2; ++it) {
      ++d;
    }
    return d;
  };
}


#endif /* SKIN_HELPERS_HPP */
