PROJECT(CTKScriptingPythonCore)


#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_SCRIPTING_PYTHON_CORE_EXPORT")

  
# Source files
SET(KIT_SRCS
  ctkAbstractPythonManager.cpp
  ctkAbstractPythonManager.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkAbstractPythonManager.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

#
# Configure file describing which Qt modules are wrapped
#
FOREACH(qtlib GUI NETWORK OPENGL SQL SVG UITOOLS WEBKIT XML XMLPATTERNS)
  SET(CTK_PYTHONQT_WRAP_QT${qtlib} ${CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QT${qtlib}})
  #message(CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QT${qtlib}:${CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QT${qtlib}})
ENDFOREACH()

# TODO Ideally a file named ctkScripingPythonWidgetsConfigure.h should
# also be configured, this file should deal with Qt Module depending only on QtGui

CONFIGURE_FILE(
  ctkScriptingPythonCoreConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkScriptingPythonCoreConfigure.h
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

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

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  #ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
