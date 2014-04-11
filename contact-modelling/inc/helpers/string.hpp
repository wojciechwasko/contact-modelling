#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <string>

namespace helpers {
  namespace string {
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
  }
}

#endif /* STRING_HPP */
