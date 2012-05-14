project(ctkQtTesting)

#
# 3rd party dependencies
#
find_package(QtTesting REQUIRED)
include_directories(
  ${QtTesting_INCLUDE_DIRS}
  ${CTKQtTesting_INCLUDE_DIRS}
  ${CTK_BASE_INCLUDE_DIRS}
  )

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ctkQtTestingMain.cpp
  ctkQtTestingMainWindow.cpp
  ctkQtTestingMainWindow.h
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkQtTestingMainWindow.h
)

# UI files
set(KIT_UI_FORMS
  ctkQtTestingMainWindow.ui
)

# Resources
set(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)
list(APPEND KIT_target_libraries CTKQtTesting)

ctkMacroBuildApp(
  NAME ${PROJECT_NAME}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )

# Testing
if(BUILD_TESTING)
#   add_subdirectory(Testing)
endif()
