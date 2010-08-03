PROJECT(CTKScriptingPythonCore)

#
# 3rd party dependencies
#

FIND_PACKAGE(PythonQt)
IF(NOT PYTHONQT_FOUND)
  MESSAGE(FATAL_ERROR "error: PythonQt package is required to build ${PROJECT_NAME}")
ENDIF()

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_SCRIPTING_PYTHON_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  ${PYTHON_INCLUDE_DIRS}
  ${PYTHONQT_INCLUDE_DIR}
  )
  
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
FOREACH(qtlib gui network opengl sql svg uitools webkit xml xmlpatterns)
  STRING(TOUPPER ${qtlib} qtlib_uppercase)
  SET(CTK_PYTHONQT_WRAP_QT${qtlib_uppercase} ${CTK_PythonQt_Wrap_Qt${qtlib}})
ENDFOREACH()

# TODO Ideally a file named ctkScripingPythonWidgetsConfigure.h should
# also be configured, this file should deal with module depending only on QtGui

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
  INCLUDE_DIRECTORIES ${KIT_include_directories}
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
