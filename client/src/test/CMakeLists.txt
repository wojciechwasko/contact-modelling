# putting it inside src/ so that we inherit the includes/libraries/stuff
FIND_PACKAGE(Boost COMPONENTS unit_test_framework REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

ADD_EXECUTABLE(test_geometry test_geometry.cpp)
TARGET_LINK_LIBRARIES(test_geometry ${Boost_LIBRARIES})
ADD_TEST(test_geometry test_geometry)

ADD_EXECUTABLE(test_erase_by_indices test_erase_by_indices.cpp)
TARGET_LINK_LIBRARIES(test_erase_by_indices ${Boost_LIBRARIES})
ADD_TEST(test_erase_by_indices test_erase_by_indices)

ADD_EXECUTABLE(test_alg_interface test_alg_interface.cpp)
TARGET_LINK_LIBRARIES(test_alg_interface ${Boost_LIBRARIES})
ADD_TEST(test_alg_interface test_alg_interface)

ADD_EXECUTABLE(test_elastic_model test_elastic_model.cpp)
TARGET_LINK_LIBRARIES(test_elastic_model ${Boost_LIBRARIES})
ADD_TEST(test_elastic_model test_elastic_model)

ADD_EXECUTABLE(test_eq_almost test_eq_almost.cpp)
TARGET_LINK_LIBRARIES(test_eq_almost ${Boost_LIBRARIES})
ADD_TEST(test_eq_almost test_eq_almost)

ADD_EXECUTABLE(test_skin_yaml_provider test_skin_yaml_provider.cpp)
TARGET_LINK_LIBRARIES(test_skin_yaml_provider ${Boost_LIBRARIES} yaml-cpp)
ADD_TEST(test_skin_yaml_provider test_skin_yaml_provider)


ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure DEPENDS
  test_geometry
  test_erase_by_indices
  test_alg_interface
  test_elastic_model
  test_eq_almost
  test_skin_yaml_provider
)
