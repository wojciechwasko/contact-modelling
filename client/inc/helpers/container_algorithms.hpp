#ifndef CONTAINER_ALGORITHMS_HPP
#define CONTAINER_ALGORITHMS_HPP

#include <cstddef> // for size_t

namespace helpers {
  namespace container_algorithms {

    /**
     * \brief   Helper function to delete elements from a container (a vector, typically), by using
     *          a "mask" of indices to be deleted.
     * \tparam  C   container type
     * \param   cnt     container from which to delete
     * \param   indices the "mask", a vector of indices, elments at which should be deleted from
     *          cnt. Must be sorted in ascending order!
     * \param   stride  optional, proceed as if one index corresponded to stride elements, e.g.
     *                  if stride == 3, index 0 corresponds to cnt[0], cnt[1] and cnt[2]. Useful if
     *                  you have to delete values in a multidimensional-valued mesh, but have only
     *                  indices of nodes to be deleted.
     * \note    The indices in the indices vector should be unique and sorted!
     */
    template <class C>
    void erase_by_indices(C& cnt, const std::vector<size_t>& indices, const size_t stride = 1)
    {
      if (indices.size() == 0)  return;
      if (stride == 0)          return; // could probably throw here as well

      auto curr_bad = indices.begin();
      size_t index = 0;
      auto is_bad = [&](decltype(*(cnt.begin())) v) {
        const size_t unified = index/stride;
        // if we can still advance and it makes sense
        while (curr_bad+1 != indices.end() && unified > *curr_bad)
          ++curr_bad;

        ++index;

        if (unified == *curr_bad)
          return true;
        return false;
      };

      cnt.erase(std::remove_if(cnt.begin(), cnt.end(), is_bad), cnt.end());
    }
  };
};


#endif /* CONTAINER_ALGORITHMS_HPP */
