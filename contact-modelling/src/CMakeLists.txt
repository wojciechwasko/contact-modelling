# to busy to write a full find-libraries script for CMake. Who knows, maybe
# later?
execute_process(
  COMMAND skin-config --lib-cxxflags
  OUTPUT_VARIABLE SKINWARE_CXXFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE  
)
execute_process(
  COMMAND skin-config --lib-ldflags
  OUTPUT_VARIABLE SKINWARE_LDFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

FIND_PACKAGE(Boost 1.54 COMPONENTS iostreams system filesystem REQUIRED)
FIND_PACKAGE(Armadillo 2.4.2 REQUIRED)

include_directories(${ContactModellingLib_SOURCE_DIR}/inc)
include_directories(${ContactModellingSuite_SOURCE_DIR}/triangle/inc)
include_directories(${ContactModellingSuite_SOURCE_DIR}/yaml-cpp/include)
include_directories(${ContactModellingSuite_SOURCE_DIR}/gnuplot-iostream/inc)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Armadillo_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKINWARE_CXXFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SKINWARE_LDFLAGS}")

# ugly hack to enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(ContactModelling
  AlgDisplacementsToForces.cpp
  AlgForcesToDisplacements.cpp
  AlgInterface.cpp
  Delaunay.cpp
  helpers/elastic_linear_model.cpp
  helpers/log.cpp
  helpers/plot.cpp
  InterpolatorInterface.cpp
  InterpolatorLinearDelaunay.cpp
  MeshInterface.cpp
  MeshNatural.cpp
  MeshRegularSquare.cpp
  SensorValuesConverter.cpp
  SkinProviderInterface.cpp
  SkinYamlProvider.cpp
)

target_link_libraries(ContactModelling
  triangle
  yaml-cpp
  ${Boost_LIBRARIES}
  ${ARMADILLO_LIBRARIES}
)
