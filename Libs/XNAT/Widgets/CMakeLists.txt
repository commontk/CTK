project(CTKXNATWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_XNAT_WIDGETS_EXPORT")

set(KIT_SRCS
  ctkXnatLoginDialog.cpp
  ctkXnatLoginProfile.cpp
  ctkXnatProjectListModel.cpp
  ctkXnatSettings.cpp
  )

# Files which should be processed by Qts moc
set(KIT_MOC_SRCS
  ctkXnatLoginDialog.h
  ctkXnatProjectListModel.h
)


# UI forms
set(KIT_UI_FORMS
  ctkXnatLoginDialog.ui
)

# Resources
set(KIT_resources
)

set(QT_USE_QTSCRIPT 1)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
set(KIT_target_libraries)

ctkFunctionGetTargetLibraries(KIT_target_libraries)

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

## Testing
#if(BUILD_TESTING)
#  add_subdirectory(Testing)
#
#  # Compile source code snippets
#  add_subdirectory(Documentation/Snippets)
#endif()
