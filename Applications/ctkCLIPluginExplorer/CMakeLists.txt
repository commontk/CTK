project(ctkCLIPluginExplorer)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ctkCLIPluginExplorerMain.cpp
  ctkCLIPluginExplorerMainWindow.h
  ctkCLIPluginExplorerMainWindow.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkCLIPluginExplorerMainWindow.h
)

# UI files
set(KIT_UI_FORMS
  ctkCLIPluginExplorerMainWindow.ui
)

# Resources
set(KIT_resources
  resources/ctkCLIPluginExplorer.qrc
)

set(QT_USE_QTUITOOLS 1)
include(${QT_USE_FILE})

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

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})

# Testing
if(BUILD_TESTING)
#   add_subdirectory(Testing)
endif()
