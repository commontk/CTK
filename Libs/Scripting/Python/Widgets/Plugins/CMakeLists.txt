project(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtPlugin.cmake for details
#

set(PLUGIN_export_directive "CTK_SCRIPTING_PYTHON_WIDGETS_PLUGINS_EXPORT")

# Source files
set(PLUGIN_SRCS
  ctkScriptingPythonWidgetsPlugins.cpp
  ctkScriptingPythonWidgetsPlugins.h
  ctkScriptingPythonWidgetsAbstractPlugin.cpp
  ctkScriptingPythonWidgetsAbstractPlugin.h

  ctkPythonConsolePlugin.cpp
  ctkPythonConsolePlugin.h
  )

# Resources
set(PLUGIN_resources
  Resources/ctkScriptingPythonWidgetsPlugins.qrc
)

# Target libraries
set(PLUGIN_target_libraries
  CTKScriptingPythonWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
