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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SKINWARE_CXXFLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SKINWARE_LDFLAGS}")

include_directories(${ContactModelling_SOURCE_DIR}/client/inc)

# From CMake manual:
#   (We do not recommend using GLOB to collect a list of source files from your
#   source tree. If no CMakeLists.txt file changes when a source is added or
#   removed then the generated build system cannot know when to ask CMake to
#   regenerate.)
add_executable(client 
  main.cpp)
