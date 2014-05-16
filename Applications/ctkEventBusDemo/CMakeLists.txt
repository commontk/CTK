project(ctkEventBusDemo)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ctkEventBusDemoMain.cpp
  ctkEventBusDemoMainWindow.cpp
  ctkEventBusDemoMainWindow.h
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkEventBusDemoMainWindow.h
)

# UI files
set(KIT_UI_FORMS
  ctkEventBusDemoMainWindow.ui
)

# Resources
set(KIT_resources

)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

# Fix this after having discussed include dependencies with Jc
foreach(_dep ${${PROJECT_NAME}_DEPENDENCIES})
  include_directories(${${_dep}_SOURCE_DIR} ${${_dep}_BINARY_DIR})
endforeach()

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
