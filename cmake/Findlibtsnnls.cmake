# - Try to find libtsnnls (fast solver for least-squares problems Ax = b where x .> 0)
# Once done this will define
#  LIBTSNNLS_FOUND - System has libtsnnls
#  LIBTSNNLS_INCLUDE_DIRS - The libtsnnls include directories
#  LIBTSNNLS_LIBRARIES - The libraries needed to use libtsnnls
#  LIBTSNNLS_DEFINITIONS - Compiler switches required for using libtsnnls

find_path(LIBTSNNLS_INCLUDE_DIR libtsnnls/tsnnls.h)
#          PATH_SUFFIXES libxml2 )

find_library(LIBTSNNLS_LIBRARY NAMES tsnnls libtsnnls)
#             HINTS ${PC_LIBXML_LIBDIR} ${PC_LIBXML_LIBRARY_DIRS} )

set(LIBTSNNLS_LIBRARIES ${LIBTSNNLS_LIBRARY} )
set(LIBTSNNLS_INCLUDE_DIRS ${LIBTSNNLS_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBTSNNLS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(libtsnnls  DEFAULT_MSG
  LIBTSNNLS_LIBRARY LIBTSNNLS_INCLUDE_DIR)

mark_as_advanced(LIBTSNNLS_INCLUDE_DIR LIBTSNNLS_LIBRARY )
