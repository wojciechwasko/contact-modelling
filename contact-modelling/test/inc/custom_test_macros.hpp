#ifndef CUSTOM_TEST_MACROS_HPP
#define CUSTOM_TEST_MACROS_HPP

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


#endif /* CUSTOM_TEST_MACROS_HPP */
