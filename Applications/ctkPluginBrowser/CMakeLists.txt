project(ctkPluginBrowser)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ctkPluginBrowser.cpp
  ctkPluginBrowserEditors.cpp
  ctkPluginBrowserMain.cpp
  ctkPluginTableModel.cpp
  ctkPluginResourcesTreeModel.cpp
  ctkQtResourcesTreeModel.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkPluginBrowser.h
  ctkPluginBrowserEditors.h
  ctkPluginTableModel.h
  )

# UI files
set(KIT_UI_FORMS
  ctkPluginBrowserMainWindow.ui
)

# Resources
set(KIT_resources
  ctkPluginBrowserResources.qrc
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

list(APPEND KIT_target_libraries Qt${CTK_QT_VERSION}::Widgets)

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
