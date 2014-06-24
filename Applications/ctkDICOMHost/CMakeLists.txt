PROJECT(ctkDICOMHost)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

# Source files
SET(KIT_SRCS
  ctkDICOMHostMain.cpp
  ctkDICOMHostMainLogic.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkDICOMHostMainLogic.h
  )

# UI files
SET(KIT_UI_FORMS
  ctkDICOMHostMainWidget.ui
)

# Resources
SET(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildApp(
  NAME ${PROJECT_NAME}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
