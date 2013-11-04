project(ctkDICOM)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

# Source files
set(KIT_SRCS
  ctkDICOMMain.cpp
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
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
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
