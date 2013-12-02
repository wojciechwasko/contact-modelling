# No time to rewrite to CMakeLists.txt now, will maybe do it later
add_custom_target(
  driver
  ALL
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
