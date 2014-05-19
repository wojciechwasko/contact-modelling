#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <string>

/**
 * \cond DEV
 */

/**
 * \file
 * \brief   String utilities.
 */

namespace cm {
namespace details {

/**
 * \brief   A string-building utility.
 *
 * One of my favourites. Allows strings to be created through a
 * stringstream-like interface.
 *
 * Use case:
 *
 *     throw std::runtime_error(sb()
 *       << "This is some error, because val is: " << val
 *       << " and it's supposed to be: " << correct_val
 *       << ". Fix it and call me back!"
 *     );
 *
 */
struct sb
{
  /**
   * \brief   Internal "buffer"
   */
  std::stringstream ss;

  /**
   * \brief   Operator for appending to the buffer.
   */
  template<typename T>
  sb & operator << (const T& rhs)
  {
    ss << rhs;
    return *this;
  }

  /**
   * \brief   Casting to std::string
   *
   * Thanks to that, you don't have to write code like this every time:
   *
   *     ((sb() << "some text").str())
   */
  operator std::string() { return ss.str(); }

  /**
   * \brief   Convert to std::string, if you're that kind of person.
   * \sa  operator std::string()
   */
  std::string str() { return ss.str(); }
};

} /* namespace details */
} /* namespace cm */

/**
 * \endcond
 */

#endif /* STRING_HPP */
