project(CTKScriptingPythonWidgets)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_SCRIPTING_PYTHON_WIDGETS_EXPORT")


# Source files
set(KIT_SRCS
  ctkPythonConsole.cpp
  ctkPythonConsole.h
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkPythonConsole.h
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
  Resources/ctkScriptingPythonWidgets.qrc
  )

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
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

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Plugins
if(CTK_BUILD_QTDESIGNER_PLUGINS)
  add_subdirectory(Plugins)
endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
