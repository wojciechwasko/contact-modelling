# FIXME To be removed (if possible) once SkinWare and other stuff is abstracted away

FIND_PACKAGE(Boost 1.52 COMPONENTS iostreams system filesystem program_options REQUIRED)
FIND_PACKAGE(Armadillo 2.4.2 REQUIRED)

include_directories(${ContactModellingClients_SOURCE_DIR}/inc)
include_directories(${triangle_SOURCE_DIR}/inc)
include_directories(${YAML_CPP_SOURCE_DIR}/include)
include_directories(${gnuplot_iostream_SOURCE_DIR}/inc)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${Armadillo_INCLUDE_DIR})

# FIXME To be removed -- END

# ugly hack to enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

include_directories(${ContactModellingLib_SOURCE_DIR}/inc)
# From CMake manual:
#   (We do not recommend using GLOB to collect a list of source files from your
#   source tree. If no CMakeLists.txt file changes when a source is added or
#   regenerate.)


add_executable(reconstruction
  construct_suite.cpp
  reconstruction.cpp
  istream_specialisations.cpp
  process_options.cpp
)

target_link_libraries(reconstruction
  ContactModelling
  ${Boost_LIBRARIES}
)
