FIND_PACKAGE(Boost 1.52 COMPONENTS iostreams system filesystem REQUIRED)
FIND_PACKAGE(Armadillo 2.4.2 REQUIRED)
FIND_PACKAGE(libtsnnls REQUIRED)

include_directories(${ContactModellingLib_SOURCE_DIR}/inc)
include_directories(${triangle_SOURCE_DIR}/inc)
include_directories(${YAML_CPP_SOURCE_DIR}/include)
include_directories(${gnuplot_iostream_SOURCE_DIR}/inc)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Armadillo_INCLUDE_DIR})

# ugly hack to enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(ContactModelling
  AlgDisplacementsToForces.cpp
  AlgDisplacementsToNonnegativeNormalForces.cpp
  AlgDisplacementsToPressures.cpp
  AlgDisplacementsToNonnegativePressures.cpp
  AlgForcesToDisplacements.cpp
  AlgInterface.cpp
  AlgPressuresToDisplacements.cpp
  Delaunay.cpp
  helpers/elastic_linear_model.cpp
  helpers/log.cpp
  helpers/plot.cpp
  InterpolatorInterface.cpp
  InterpolatorLinearDelaunay.cpp
  MeshInterface.cpp
  MeshNatural.cpp
  MeshRegularRectangular.cpp
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
  tsnnls
)
