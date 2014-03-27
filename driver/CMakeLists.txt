# No time to rewrite to CMakeLists.txt now, will maybe do it later

SET(CUSTOM_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/driver-build")


add_custom_target(
  driver-copy
  COMMAND ${CMAKE_COMMAND} -E remove_directory
    "${CUSTOM_BUILD_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}
    ${CUSTOM_BUILD_DIR}
)

add_custom_target(
  driver
  ALL
  COMMAND make
  WORKING_DIRECTORY ${CUSTOM_BUILD_DIR}
)

ADD_DEPENDENCIES(driver driver-copy)
