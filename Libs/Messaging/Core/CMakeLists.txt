PROJECT(CTKMessagingCore)

#
# 3rd party dependencies
#

FIND_PACKAGE(ZMQ)
# TODO In case ZMQ is passed directly using ZMQ_DIR, check expected options
IF(NOT ZMQ_FOUND)
  MESSAGE(FATAL_ERROR "error: ZMQ package is required to build ${PROJECT_NAME}")
ENDIF()
INCLUDE(${ZMQ_USE_FILE})


#FIND_PACKAGE(OpenIGTLink)
# TODO In case OpenIGTLink is passed directly using OpenIGTLink_DIR, check expected options
#IF(NOT OpenIGTLink_FOUND)
#  MESSAGE(FATAL_ERROR "error: OpenIGTLink package is required to build ${PROJECT_NAME}")
#ENDIF()
#INCLUDE(${OpenIGTLink_USE_FILE})


#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_MESSAGING_CORE_EXPORT")

  
# Source files
SET(KIT_SRCS
  ctkMessagingServer.h
  ctkMessagingServer.cpp
  ctkMessagingClient.h
  ctkMessagingClient.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  )

# UI files
SET(KIT_UI_FORMS
  )

# Resources
SET(KIT_resources
  )

# Set VTK_LIBRARIES variable
#SET(ZMQ_LIBRARIES
#  vtkCommon
#  vtkFiltering
#  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
