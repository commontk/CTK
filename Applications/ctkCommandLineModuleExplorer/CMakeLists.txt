project(ctkCommandLineModuleExplorer)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ctkCommandLineModuleExplorerMain.cpp
  ctkCmdLineModuleExplorerConstants.cpp
  ctkCmdLineModuleExplorerDirectorySettings.cpp
  ctkCmdLineModuleExplorerGeneralModuleSettings.cpp
  ctkCmdLineModuleExplorerMainWindow.h
  ctkCmdLineModuleExplorerMainWindow.cpp
  ctkCmdLineModuleExplorerModulesSettings.cpp
  ctkCmdLineModuleExplorerOutputText.cpp
  ctkCmdLineModuleExplorerProgressListWidget.cpp
  ctkCmdLineModuleExplorerProgressWidget.cpp
  ctkCmdLineModuleExplorerShowXmlAction.cpp
  ctkCmdLineModuleExplorerTabList.cpp
  ctkCmdLineModuleExplorerTreeWidget.cpp
  ctkCmdLineModuleExplorerUtils.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkCmdLineModuleExplorerDirectorySettings.h
  ctkCmdLineModuleExplorerGeneralModuleSettings.h
  ctkCmdLineModuleExplorerMainWindow.h
  ctkCmdLineModuleExplorerModulesSettings.h
  ctkCmdLineModuleExplorerOutputText.h
  ctkCmdLineModuleExplorerProgressListWidget.h
  ctkCmdLineModuleExplorerProgressWidget.h
  ctkCmdLineModuleExplorerShowXmlAction.h
  ctkCmdLineModuleExplorerTabList.h
  ctkCmdLineModuleExplorerTreeWidget.h
)

# UI files
set(KIT_UI_FORMS
  ctkCmdLineModuleExplorerDirectorySettings.ui
  ctkCmdLineModuleExplorerGeneralModuleSettings.ui
  ctkCmdLineModuleExplorerMainWindow.ui
  ctkCmdLineModuleExplorerModulesSettings.ui
  ctkCmdLineModuleExplorerProgressWidget.ui
)

# Resources
set(KIT_resources
  resources/ctkCmdLineModuleExplorer.qrc
)

if(CTK_QT_VERSION VERSION_LESS "5")
  set(QT_USE_QTUITOOLS 1)
  include(${QT_USE_FILE})
endif()

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
