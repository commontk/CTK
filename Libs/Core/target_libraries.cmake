#
# See CMake/ctkFunctionGetTargetLibraries.cmake
#
# This file should list the libraries required to build the current CTK libraries
#

set(target_libraries
  BFD_LIBRARIES
  )

if (CTK_QT_VERSION VERSION_LESS "5")
  list(APPEND target_libraries QT_LIBRARIES)
endif()
