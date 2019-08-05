project(CTKScriptingPythonCore)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_SCRIPTING_PYTHON_CORE_EXPORT")

# Source files
set(KIT_SRCS
  ctkAbstractPythonManager.cpp
  ctkAbstractPythonManager.h
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkAbstractPythonManager.h
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)

#
# Configure file describing which Qt modules are wrapped
#
set(QT_PYTHON_WRAPPED_LIBRARIES_LIST Core Gui Widgets Network OpenGL Sql Svg UiTools WebKit WebKitWidgets Xml XmlPatterns)
foreach(qtlib ${QT_PYTHON_WRAPPED_LIBRARIES_LIST})
  string(TOUPPER ${qtlib} qtlib)
  set(CTK_PYTHONQT_WRAP_QT${qtlib} ${CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QT${qtlib}})
endforeach()

configure_file(
  ctkScriptingPythonCoreConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkScriptingPythonCoreConfigure.h
  )

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)
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

# Plugins
#if(CTK_BUILD_QTDESIGNER_PLUGINS)
#  add_subdirectory(Plugins)
#endif()

add_subdirectory(Python)

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
