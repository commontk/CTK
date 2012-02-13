#
# PythonQtGenerator
#

if(CTK_WRAP_PYTHONQT_FULL)

  # Sanity checks
  if(DEFINED PYTHONQTGENERATOR_EXECUTABLE AND NOT EXISTS ${PYTHONQTGENERATOR_EXECUTABLE})
    message(FATAL_ERROR "PYTHONQTGENERATOR_EXECUTABLE variable is defined but corresponds to non-existing executable")
  endif()

  set(proj PythonQtGenerator)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  if(CTK_SUPERBUILD)

    if(NOT DEFINED PYTHONQTGENERATOR_EXECUTABLE)

      #
      # PythonQtGenerator is the tool allowing to generate the PythonQt decorators using
      # typesystem xml files. If called without any option, it will generate the bindings for Qt.
      #
      # See http://www.pyside.org/docs/apiextractor/typesystem.html
      # See http://doc.trolltech.com/qtjambi-4.5.2_01/com/trolltech/qt/qtjambi-typesystem.html
      # See http://qt.gitorious.org/qt-labs/qtscriptgenerator
      #

    #   message(STATUS "Adding project:${proj}")

      #
      # If directory PythonQt already exists, the corresponding project will
      # be cloned. Since PythonQt and PythonQtGenerator source code are hosted
      # on the same repository, we will assume that if the directory PythonQt
      # exists, the generator code will also be available.
      #
      if(EXISTS ${CMAKE_BINARY_DIR}/PythonQt)
        #message(STATUS "ExternalProject/PythonQtGenerator: PythonQt already added as ExternalProject")
        ExternalProject_Add(${proj}
          SOURCE_DIR ${CMAKE_BINARY_DIR}/PythonQt/generator
          BINARY_DIR ${proj}-build
          PREFIX ${proj}${ep_suffix}
          DOWNLOAD_COMMAND ""
          CMAKE_GENERATOR ${gen}
          INSTALL_COMMAND ""
          CMAKE_CACHE_ARGS
            ${ep_common_cache_args}
            -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
          DEPENDS
            "PythonQt" # To make sure the generator code is checked out, let's depend on PythonQt
          )
      else()

        set(revision_tag 3171a94e16ba9bfee137)
        if(${proj}_REVISION_TAG)
          set(revision_tag ${${proj}_REVISION_TAG})
        endif()

        #message(STATUS "ExternalProject/PythonQtGenerator: PythonQt is NOT an ExternalProject")
        ExternalProject_Add(${proj}
          SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
          BINARY_DIR ${proj}-build
          PREFIX ${proj}${ep_suffix}
          GIT_REPOSITORY "${git_protocol}://github.com/commontk/PythonQt.git"
          GIT_TAG 3171a94e16ba9bfee137
          CMAKE_GENERATOR ${gen}
          UPDATE_COMMAND ""
          INSTALL_COMMAND ""
          CMAKE_CACHE_ARGS
            ${ep_common_cache_args}
            -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
          )
      endif()

      set(PYTHONQTGENERATOR_EXECUTABLE ${CMAKE_BINARY_DIR}/PythonQtGenerator-build/PythonQtGenerator)

      # Since PythonQtGenerator is an executable, there is no need to add its corresponding
      # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS PYTHONQTGENERATOR_EXECUTABLE:FILEPATH)

  endif()

endif()
