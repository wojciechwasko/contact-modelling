#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <memory>

namespace helpers {
  namespace memory {

    /**
     * \brief   A small, nifty RAII-style deallocator for raw pointers.
     *
     * A writeup for this class is available at
     * http://blog.merlotatnight.com/2014/03/hacking-raii-deleter.html
     *
     * To put it simply, it takes a raw pointer on construction; when it's destructed (possibly by
     * going out of scope), it'll call a delete on the raw pointer. Note that if you want to pass a
     * pointer which is beginning of an array, not merely a single instance of a type, you have to
     * specify type[] as the second template parameter to this class. Example:
     * \code
     * // single instance of the type
     * int *t = new int(4);   delguard<int> dt(t);
     * // an array
     * int *a = new int[10];  delguard<int,int[]> da(a);
     * \endcode
     */
    template <class T, class delT = T>
    class delguard {          
      T *& what_;
    public:
      delguard(delguard const&)             = delete;
      delguard& operator=(delguard const&)  = delete;
      delguard(delguard&&)                  = delete;
      delguard& operator=(delguard&&)       = delete;
      delguard(T *& what) : what_(what) {}
      ~delguard() {
        std::default_delete<delT> actual_del;
        actual_del(what_);
        what_ = nullptr;
      }
    };
  }
}

#endif /* MEMORY_HPP */
