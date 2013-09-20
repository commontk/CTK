#
# See CMake/ctkFunctionGetTargetLibraries.cmake
# 
# This file should list the libraries required to build the current CTK libraries
#

set(target_libraries
  CTKCore
  )
if (CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND target_libraries Qt5Xml_LIBRARIES Qt5OpenGL_LIBRARIES)
endif()