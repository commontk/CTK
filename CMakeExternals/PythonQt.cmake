#
# PythonQt
#
SET(PythonQt_DEPENDS)
ctkMacroShouldAddExternalProject(PYTHONQT_LIBRARIES add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED PYTHONQT_INSTALL_DIR AND NOT EXISTS ${PYTHONQT_INSTALL_DIR})
    MESSAGE(FATAL_ERROR "PYTHONQT_INSTALL_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  IF(NOT DEFINED PYTHONQT_INSTALL_DIR)
    SET(proj PythonQt)
  #   MESSAGE(STATUS "Adding project:${proj}")
    SET(PythonQt_DEPENDS ${proj})

    # Python is required
    FIND_PACKAGE(PythonLibs)
    IF(NOT PYTHONLIBS_FOUND)
      MESSAGE(FATAL_ERROR "error: Python is required to build ${PROJECT_NAME}")
    ENDIF()
      
    ExternalProject_Add(${proj}
      GIT_REPOSITORY "git://github.com/commontk/PythonQt.git"
      GIT_TAG "patched"
      CMAKE_GENERATOR ${gen}
      BUILD_COMMAND ""
      CMAKE_ARGS
        ${ep_common_args}
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}
        -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
      )
    SET(PYTHONQT_INSTALL_DIR ${ep_install_dir})
    
    # Since the full path of PythonQt library is used, there is not need to add 
    # its corresponding library output directory to CTK_EXTERNAL_LIBRARY_DIRS
    
  ENDIF()
ENDIF()
