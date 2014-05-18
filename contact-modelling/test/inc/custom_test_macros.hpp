#ifndef CUSTOM_TEST_MACROS_HPP
#define CUSTOM_TEST_MACROS_HPP

#include <string>
#include <stdexcept>

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
        BOOST_CHECK_CLOSE(*a, *b, tolerance); \
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
