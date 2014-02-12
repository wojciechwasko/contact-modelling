cmake_minimum_required (VERSION 2.6) 
project (ContactModelling) 
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. Please go to build/
directory and build there (cd build/ && cmake .. && make)")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

add_subdirectory (client) 
add_subdirectory (driver)

