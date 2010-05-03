# - Find a CTK installation or build tree.
# The following variables are set if CTK is found.  If CTK is not
# found, CTK_FOUND is set to false.
#  CTK_FOUND         - Set to true when VTK is found.
#  CTK_USE_FILE      - CMake file to use VTK.
#  CTK_MAJOR_VERSION - The VTK major version number.
#  CTK_MINOR_VERSION - The VTK minor version number
#                       (odd non-release).
#  CTK_BUILD_VERSION - The VTK patch level
#                       (meaningless for odd minor).
#  CTK_INCLUDE_DIRS  - Include directories for CTK
#  CTK_LIBRARY_DIRS  - Link directories for CTK libraries
#
# The following cache entries must be set by the user to locate VTK:
#  CTK_DIR  - The directory containing CTKConfig.cmake.
#             This is either the root of the build tree,
#             or the lib/vtk directory.  This is the 
#             only cache entry.
#

# Assume not found.
SET(CTK_FOUND 0)

# Construct consitent error messages for use below.
SET(CTK_DIR_MESSAGE "CTK not found.  Set the CTK_DIR cmake cache entry to the directory containing CTKConfig.cmake.  This is either the root of the build tree, or PREFIX/lib/vtk for an installation.")

# Use the Config mode of the find_package() command to find CTKConfig.
# If this succeeds (possibly because CTK_DIR is already set), the
# command will have already loaded CTKConfig.cmake and set CTK_FOUND.
IF(NOT CTK_FOUND)
  FIND_PACKAGE(CTK QUIET NO_MODULE)
ENDIF()

#-----------------------------------------------------------------------------
IF(NOT CTK_FOUND)
  # CTK not found, explain to the user how to specify its location.
  IF(CTK_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR ${CTK_DIR_MESSAGE})
  ELSE(CTK_FIND_REQUIRED)
    IF(NOT CTK_FIND_QUIETLY)
      MESSAGE(STATUS ${CTK_DIR_MESSAGE})
    ENDIF(NOT CTK_FIND_QUIETLY)
  ENDIF(CTK_FIND_REQUIRED)
ENDIF()