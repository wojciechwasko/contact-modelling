set(DATA_TARGET_DIR ${PROJECT_BINARY_DIR}/data-copy)

add_custom_target(
  data-copy
  ALL
  COMMAND ${CMAKE_COMMAND} -E remove_directory
  "${DATA_TARGET_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}
  ${DATA_TARGET_DIR}
)
