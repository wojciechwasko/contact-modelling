include_directories(${ContactModelling_SOURCE_DIR}/triangle/inc)
add_definitions(-DTRILIBRARY)
add_library(triangle triangle.c)
