#
# PythonQtGenerator
#

set(proj PythonQtGenerator)

# Sanity checks
if(DEFINED PYTHONQTGENERATOR_EXECUTABLE AND NOT EXISTS ${PYTHONQTGENERATOR_EXECUTABLE})
  message(FATAL_ERROR "PYTHONQTGENERATOR_EXECUTABLE variable is defined but corresponds to non-existing executable")
endif()

set(${proj}_DEPENDENCIES PythonQt)

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  DEPENDS_VAR ${proj}_DEPENDENCIES
  EP_ARGS_VAR ${proj}_EXTERNAL_PROJECT_ARGS
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

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
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/PythonQt/generator
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    DOWNLOAD_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES} # To make sure the generator code is checked out, let's depend on PythonQt
    )

  set(PYTHONQTGENERATOR_EXECUTABLE ${CMAKE_BINARY_DIR}/PythonQtGenerator-build/PythonQtGenerator)

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS PYTHONQTGENERATOR_EXECUTABLE:FILEPATH
  LABELS "FIND_PACKAGE"
  )
