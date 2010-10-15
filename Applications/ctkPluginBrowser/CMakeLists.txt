PROJECT(ctkPluginBrowser)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

SET(KIT_SRCS
  ctkPluginBrowser.cpp
  ctkPluginBrowserEditors.cpp
  ctkPluginBrowserMain.cpp
  ctkPluginTableModel.cpp
  ctkPluginResourcesTreeModel.cpp
  ctkQtResourcesTreeModel.cpp
)

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkPluginBrowser.h
  ctkPluginBrowserEditors.h
  ctkPluginTableModel.h
  )

# UI files
SET(KIT_UI_FORMS
  ctkPluginBrowserMainWindow.ui
)

# Resources
SET(KIT_resources
  ctkPluginBrowserResources.qrc
)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

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
#   ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
