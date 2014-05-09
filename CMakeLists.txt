cmake_minimum_required (VERSION 2.6) 
project (ContactModellingSuite) 
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed. Please go to build/
directory and build there (cd build/ && cmake .. && make)")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

ENABLE_TESTING()
add_subdirectory (triangle)
add_subdirectory (yaml-cpp)
add_subdirectory (data)
add_subdirectory (contact-modelling)
add_subdirectory (cm-clients) 
