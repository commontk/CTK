#
# PythonQtGenerator
#

SET(PythonQtGenerator_DEPENDS)
IF(CTK_WRAP_PYTHONQT_FULL)
  
  # Sanity checks
  IF(DEFINED PYTHONQTGENERATOR_EXECUTABLE AND NOT EXISTS ${PYTHONQTGENERATOR_EXECUTABLE})
    MESSAGE(FATAL_ERROR "PYTHONQTGENERATOR_EXECUTABLE variable is defined but corresponds to non-existing executable")
  ENDIF()
  
  IF(NOT DEFINED PYTHONQTGENERATOR_EXECUTABLE)
    
    #
    # PythonQtGenerator is the tool allowing to generate the PythonQt decorators using 
    # typesystem xml files. If called without any option, it will generate the bindings for Qt.
    #
    # See http://www.pyside.org/docs/apiextractor/typesystem.html
    # See http://doc.trolltech.com/qtjambi-4.5.2_01/com/trolltech/qt/qtjambi-typesystem.html
    # See http://qt.gitorious.org/qt-labs/qtscriptgenerator
    #
    SET(proj PythonQtGenerator)
  #   MESSAGE(STATUS "Adding project:${proj}")
    SET(PythonQtGenerator_DEPENDS ${proj})
    
    #
    # If directory PythonQt already exists, the corresponding project will 
    # be cloned. Since PythonQt and PythonQtGenerator source code are hosted 
    # on the same repository, we will assume that if the directory PythonQt 
    # exists, the generator code will also be available.
    #
    IF(EXISTS ${CMAKE_BINARY_DIR}/PythonQt)
      #MESSAGE(STATUS "ExternalProject/PythonQtGenerator: PythonQt already added as ExternalProject")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        DOWNLOAD_COMMAND ""
        CMAKE_GENERATOR ${gen}
        INSTALL_COMMAND ""
        SOURCE_DIR ${CMAKE_BINARY_DIR}/PythonQt/generator
        CMAKE_ARGS
          ${ep_common_args}
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        DEPENDS
          "PythonQt" # To make sure the generator code is checked out, let's depent on PythonQt
        )
    ELSE()
      #MESSAGE(STATUS "ExternalProject/PythonQtGenerator: PythonQt is NOT an ExternalProject")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        GIT_REPOSITORY "${git_protocol}://github.com/commontk/PythonQt.git"
        GIT_TAG "patched"
        CMAKE_GENERATOR ${gen}
        INSTALL_COMMAND ""
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}/generator
        CMAKE_ARGS
          ${ep_common_args}
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        )
    ENDIF()
    
    SET(PYTHONQTGENERATOR_EXECUTABLE ${CMAKE_BINARY_DIR}/PythonQtGenerator/PythonQtGenerator)
    
    # Since PythonQtGenerator is an executable, there is no need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
        
  ENDIF()
  
  LIST(APPEND CTK_SUPERBUILD_EP_ARGS -DPYTHONQTGENERATOR_EXECUTABLE:FILEPATH=${PYTHONQTGENERATOR_EXECUTABLE})
  
ENDIF()
