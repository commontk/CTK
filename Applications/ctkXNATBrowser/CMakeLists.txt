project(ctkXNATBrowser)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ctkXNATBrowserMain.cpp
  ctkXNATBrowserMainWindow.cpp
)

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkXNATBrowserMainWindow.h
)

# UI files
set(KIT_UI_FORMS
  ctkXNATBrowserMainWindow.ui
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

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
