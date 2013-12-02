#
# PythonQtGenerator
#

superbuild_include_once()

# Sanity checks
if(DEFINED PYTHONQTGENERATOR_EXECUTABLE AND NOT EXISTS ${PYTHONQTGENERATOR_EXECUTABLE})
  message(FATAL_ERROR "PYTHONQTGENERATOR_EXECUTABLE variable is defined but corresponds to non-existing executable")
endif()

set(PythonQtGenerator_DEPENDENCIES PythonQt)

ctkMacroCheckExternalProjectDependency(PythonQtGenerator)
set(proj PythonQtGenerator)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")
endif()

if(NOT DEFINED PYTHONQTGENERATOR_EXECUTABLE)

  #
  # PythonQtGenerator is the tool allowing to generate the PythonQt decorators using
  # typesystem xml files. If called without any option, it will generate the bindings for Qt.
  #
  # See http://www.pyside.org/docs/apiextractor/typesystem.html
  # See http://doc.trolltech.com/qtjambi-4.5.2_01/com/trolltech/qt/qtjambi-typesystem.html
  # See http://qt.gitorious.org/qt-labs/qtscriptgenerator
  #

  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/PythonQt/generator
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    DOWNLOAD_COMMAND ""
    CMAKE_GENERATOR ${gen}
    INSTALL_COMMAND ""
    LIST_SEPARATOR ${sep}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES} # To make sure the generator code is checked out, let's depend on PythonQt
    )

  set(PYTHONQTGENERATOR_EXECUTABLE ${CMAKE_BINARY_DIR}/PythonQtGenerator-build/PythonQtGenerator)

  # Since PythonQtGenerator is an executable, there is no need to add its corresponding
  # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

else()
  ctkMacroEmptyExternalproject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND CTK_SUPERBUILD_EP_VARS PYTHONQTGENERATOR_EXECUTABLE:FILEPATH)
