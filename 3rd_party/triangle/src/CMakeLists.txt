include_directories(${triangle_SOURCE_DIR}/inc)
add_definitions(-DTRILIBRARY)
add_library(triangle triangle.c)
