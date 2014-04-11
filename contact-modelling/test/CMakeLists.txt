# putting it inside src/ so that we inherit the includes/libraries/stuff
FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

INCLUDE_DIRECTORIES(${ContactModellingLib_SOURCE_DIR}/inc)
include_directories(${ContactModellingSuite_SOURCE_DIR}/yaml-cpp/include)
# ugly hack to enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ADD_EXECUTABLE(test_geometry test_geometry.cpp)
TARGET_LINK_LIBRARIES(test_geometry 
  ContactModelling
  ${Boost_LIBRARIES})
ADD_TEST(test_geometry test_geometry)

ADD_EXECUTABLE(test_erase_by_indices test_erase_by_indices.cpp)
TARGET_LINK_LIBRARIES(test_erase_by_indices 
  ContactModelling
  ${Boost_LIBRARIES})
ADD_TEST(test_erase_by_indices test_erase_by_indices)

ADD_EXECUTABLE(test_alg_interface test_alg_interface.cpp)
TARGET_LINK_LIBRARIES(test_alg_interface
  ContactModelling
  ${Boost_LIBRARIES})
ADD_TEST(test_alg_interface test_alg_interface)

ADD_EXECUTABLE(test_elastic_model test_elastic_model.cpp)
TARGET_LINK_LIBRARIES(test_elastic_model
  ContactModelling
  ${Boost_LIBRARIES})
ADD_TEST(test_elastic_model test_elastic_model)

ADD_EXECUTABLE(test_eq_almost test_eq_almost.cpp)
TARGET_LINK_LIBRARIES(test_eq_almost 
  ContactModelling
  ${Boost_LIBRARIES})
ADD_TEST(test_eq_almost test_eq_almost)

ADD_EXECUTABLE(test_skin_yaml_provider test_skin_yaml_provider.cpp)
TARGET_LINK_LIBRARIES(test_skin_yaml_provider
  ContactModelling
  ${Boost_LIBRARIES} yaml-cpp)
ADD_TEST(test_skin_yaml_provider test_skin_yaml_provider)

ADD_EXECUTABLE(test_interpolator_linear_delaunay test_interpolator_linear_delaunay.cpp)
TARGET_LINK_LIBRARIES(test_interpolator_linear_delaunay
  ContactModelling
  ${Boost_LIBRARIES} yaml-cpp)
ADD_TEST(test_interpolator_linear_delaunay test_interpolator_linear_delaunay)



ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure DEPENDS
  test_geometry
  test_erase_by_indices
  test_alg_interface
  test_elastic_model
  test_eq_almost
  test_skin_yaml_provider
  test_interpolator_linear_delaunay
)