#ifndef ITERATE_HPP
#define ITERATE_HPP

namespace helpers {
  namespace iterate {

    /**
     * \brief a vesion of std::for_each which will provide the function with an iterator, not the
     * value.
     *
     * While it's usually good that for_each-based algorithms don't receive the iterator, sometimes
     * it's handy to have it.
     */
    template <class I, class F>
    F for_each_it(I first, I last, F fn)
    {
      while (first!=last) {
        fn(first);
        ++first;
      }
      return fn; 
    }
  }
}


#endif /* ITERATE_HPP */
