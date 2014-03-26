# putting it inside src/ so that we inherit the includes/libraries/stuff
FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

ADD_EXECUTABLE(test_geometry test_geometry.cpp)
TARGET_LINK_LIBRARIES(test_geometry ${Boost_LIBRARIES})
ADD_TEST(test_geometry test_geometry)

ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure DEPENDS
  test_geometry
)
