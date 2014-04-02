set(CMAKE_BUILD_TYPE Debug)
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake_modules")

FIND_PACKAGE( Boost 1.54 COMPONENTS iostreams system filesystem REQUIRED )
include_directories(${ContactModelling_SOURCE_DIR}/client/inc)
include_directories(${ContactModelling_SOURCE_DIR}/triangle/inc)
include_directories(${ContactModelling_SOURCE_DIR}/gnuplot-iostream/inc)
include_directories(${Boost_INCLUDE_DIR})

link_directories(${Boost_LIBRARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKINWARE_CXXFLAGS} -O0 -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SKINWARE_LDFLAGS}")

# From CMake manual:
#   (We do not recommend using GLOB to collect a list of source files from your
#   source tree. If no CMakeLists.txt file changes when a source is added or
#   removed then the generated build system cannot know when to ask CMake to
#   regenerate.)
add_executable(client 
  main.cpp
)

target_link_libraries(client
  triangle
  ${Boost_LIBRARIES}
)

ADD_SUBDIRECTORY(test)
