#ifndef CUSTOM_TEST_MACROS_HPP
#define CUSTOM_TEST_MACROS_HPP

#include <iomanip> // for setprecision
#include <string>
#include <stdexcept>
#include <cmath>

namespace testimpl {
inline bool within_percent(double a, double b, double tol)
{
  return (a==0 && b ==0) || (std::fabs((a-b)/a) < 0.01*tol);
}

// copied over from cm/details/string.hpp
// Can't rely on the lib to keep it.
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
};

}

// rip-off from http://stackoverflow.com/a/17503436/312650
// Have to make it a macro so that it reports exact line numbers when checks fail.
#define CHECK_CLOSE_COLLECTION(aa, bb, tolerance) { \
    using std::distance; \
    using std::begin; \
    using std::end; \
    auto a = begin(aa), ae = end(aa); \
    auto b = begin(bb); \
    BOOST_REQUIRE_EQUAL(distance(a, ae), distance(b, end(bb))); \
    for(; a != ae; ++a, ++b) { \
        BOOST_CHECK_MESSAGE(testimpl::within_percent(*a,*b,tolerance), \
          (std::string) (testimpl::sb() << std::setprecision(20) \
            << "Element " << distance(begin(aa),a) \
            << ": difference {" << 100*std::fabs((*a-*b)/(*b)) << "%}" \
            << " (" << *a << " vs. " << *b << ") exceeds " << tolerance << "%")); \
    } \
}

#define CHECK_CLOSE_COLLECTION_IGNORE_SMALL(aa, bb, tolerance, small_threshold) { \
    using std::distance; \
    using std::begin; \
    using std::end; \
    auto a = begin(aa), ae = end(aa); \
    auto b = begin(bb); \
    BOOST_REQUIRE_EQUAL(distance(a, ae), distance(b, end(bb))); \
    for(; a != ae; ++a, ++b) { \
      if (std::fabs(*a) < std::fabs(small_threshold)) { \
        continue; \
      } \
        BOOST_CHECK_MESSAGE(testimpl::within_percent(*a,*b,tolerance), \
          (std::string) (testimpl::sb() << std::setprecision(20) \
            << "Element " << distance(begin(aa),a) \
            << ": difference {" << 100*std::fabs((*a-*b)/(*b)) << "%}" \
            << " (" << *a << " vs. " << *b << ") exceeds " << tolerance << "%")); \
    } \
}

#define CHECK_DOWNCAST(obj, type) { \
  try { \
    dynamic_cast<type>(obj); \
  } catch (std::bad_cast& e) { \
    BOOST_FAIL(e.what()); \
  } \
}

#define CHECK_EQUAL_STR( s, t ) \
  BOOST_CHECK_EQUAL(std::string(s), std::string(t));


#endif /* CUSTOM_TEST_MACROS_HPP */
