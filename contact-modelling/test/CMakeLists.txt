PROJECT(ContactModellingTests)
# putting it inside src/ so that we inherit the includes/libraries/stuff
FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

INCLUDE_DIRECTORIES(${ContactModellingLib_SOURCE_DIR}/inc)
INCLUDE_DIRECTORIES(${YAML_CPP_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${gnuplot_iostream_SOURCE_DIR}/inc)
INCLUDE_DIRECTORIES(${ContactModellingTests_SOURCE_DIR}/inc)
# ugly hack to enable C++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ADD_EXECUTABLE(tests
  tests_driver.cpp

  algorithm/alg_interface.cpp
  details/exception.cpp
  details/eq_almost.cpp
  details/erase_by_indices.cpp
  details/geometry.cpp
  elastic_models/forces.cpp
  elastic_models/pressures.cpp
  grid/cell_shapes.cpp
  grid/dump_grid.cpp
  grid/grid.cpp
  interpolator/linear_delaunay.cpp
  log/logging.cpp
  skin_provider/yaml.cpp
  skin_provider/luca.cpp
  skin_provider/luca_details.cpp
)
TARGET_LINK_LIBRARIES(tests
  ContactModelling
  ${Boost_LIBRARIES}
  yaml-cpp  
)
ADD_TEST(tests tests)

ADD_CUSTOM_TARGET(check COMMAND "${ContactModellingTests_BINARY_DIR}/tests" "--show_progress" DEPENDS
  tests
)
