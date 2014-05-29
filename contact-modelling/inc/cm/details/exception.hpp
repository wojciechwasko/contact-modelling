#ifndef DETAILS__EXCEPTION_HPP
#define DETAILS__EXCEPTION_HPP

#include "cm/log/log.hpp"

namespace cm {

/**
 * \cond DEV
 */
namespace details {

/**
 * \brief   Helper that is constructed with an exception and will throw it when
 * checked through check() unless disabled through disable()
 *
 * For convenience, a make_ExceptionGuard() function is provided, similar to
 * make_shared(), make_unique() from std.
 *
 * Useful for loop-based processing of streams, e.g.:
 *     
 *      std::ifstream some_stream(file_name);
 *      std::string line;
 *      auto guard = make_ExceptionGuard(SOME_EXCEPTION);
 *      while (some_stream >> line) {
 *        // process line
 *        if (not_happy_with_result)
 *          continue;
 *        save_result_somewhere();
 *        guard.disable();
 *        break;
 *      }
 * 
 * You don't need to worry about the stream being in valid state or not, simply
 * enough, if you can't find what you want in the stream, a proper exception
 * will be raised automatically. I *personally* believe it's a much cleaner
 * solution than setting some error codes and than doing `if`s on them. 
 *
 * \note The destructor doesn't throw. Throwing from destructors is a Bad Thing
 * (TM). If the guard comes to be destroyed but is still active (i.e. it wasn't
 * checked or disabled), it will write an appropriate message into the `ERROR`
 * category in log.
 */
template <class E>
class ExceptionGuard {
  /**
   * \brief   Exception to be thrown
   */
  E e_;
  /**
   * \brief   Whether the guard is active
   */
  bool enabled_;

public:
  /**
   * \brief   Construct a guard. The exception is copied into a member variable
   * \param   e   exception to be thrown
   */
  ExceptionGuard(const E& e) : e_(e), enabled_(true) {}
  /**
   * \brief   Disable the guard; if it will be checked later on, it *won't*
   * throw
   */
  void disable() { enabled_ = false; }
  /**
   * \brief   If the guard is still enabled, this will throw the exception
   * provided in the constructor.
   */
  void check() { if (enabled_) throw e_; }
  /**
   * \brief   If the guard is still active by the time it is to be deallocated,
   * an appropriate message will be written to ERROR category of log.
   */
  ~ExceptionGuard()
  {
    if (enabled_) {
      LOG(ERROR) 
        << "ExceptionGuard: would've normally thrown an exception with what(): '"
        << e_.what() << "' because I wasn't disabled, but I was never checked!"
        << "To not throw from the destructor, I'm simply loggin it to ERROR";
    }
  }
};

/**
 * \brief   Convenience function for creating exception guards.
 */
template <class E>
ExceptionGuard<E> make_ExceptionGuard(E e) { return ExceptionGuard<E>(e); }

} /* namespace details */
/**
 * \endcond
 */

} /* namespace cm */


#endif /* DETAILS__EXCEPTION_HPP */
