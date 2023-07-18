#
# See CMake/ctkFunctionGetTargetLibraries.cmake
# 
# This file should list the libraries required to build the current CTK libraries
#

set(target_libraries
  VTK_LIBRARIES
  CTKWidgets
  CTKVisualizationVTKCore
  )
if(CTK_QT_VERSION VERSION_EQUAL "5")
  list(APPEND target_libraries Qt5Network_LIBRARIES Qt5WebKit_LIBRARIES)
else()
  message(FATAL_ERROR "Support for this Qt is not implemented")
endif()
