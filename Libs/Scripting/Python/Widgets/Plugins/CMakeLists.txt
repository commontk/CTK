PROJECT(${PROJECT_NAME}Plugins)

#
# See CTK/CMake/ctkMacroBuildQtDesignerPlugin.cmake for details
#

SET(PLUGIN_export_directive "CTK_SCRIPTING_PYTHON_WIDGETS_PLUGINS_EXPORT")

# Source files
SET(PLUGIN_SRCS
  ctkScriptingPythonWidgetsPlugins.cpp
  ctkScriptingPythonWidgetsPlugins.h
  ctkScriptingPythonWidgetsAbstractPlugin.cpp
  ctkScriptingPythonWidgetsAbstractPlugin.h

  ctkPythonConsolePlugin.cpp
  ctkPythonConsolePlugin.h
  )

# Headers that should run through moc
SET(PLUGIN_MOC_SRCS
  ctkScriptingPythonWidgetsPlugins.h

  ctkPythonConsolePlugin.h
  )

# Resources
SET(PLUGIN_resources
  Resources/CTKScriptingPythonWidgetsPlugins.qrc
)

# Target libraries
SET(PLUGIN_target_libraries
  CTKScriptingPythonWidgets
  )

ctkMacroBuildQtDesignerPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)
