#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <string>

namespace cm {
namespace details {

struct sb
{
  std::stringstream ss;
  template<typename T>
  sb & operator << (const T& rhs)
  {
    ss << rhs;
    return *this;
  }
  operator std::string() { return ss.str(); }
  std::string str() { return ss.str(); }
};

} /* namespace details */
} /* namespace cm */

#endif /* STRING_HPP */
