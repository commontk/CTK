#
# PythonQt
#
SET(PythonQt_DEPENDS)
ctkMacroShouldAddExternalProject(PYTHONQT_LIBRARIES add_project)
IF(${add_project})
  IF(NOT DEFINED PYTHONQT_INSTALL_DIR)
    SET(proj PythonQt)
  #   MESSAGE(STATUS "Adding project:${proj}")
    SET(PythonQt_DEPENDS ${proj})

    # Python is required
    FIND_PACKAGE(PythonLibs)
    IF(NOT PYTHONLIBS_FOUND)
      MESSAGE(FATAL_ERROR "error: Python is required to build ${PROJECT_NAME}")
    ENDIF()

    # Patch program
    FIND_PROGRAM(CTK_PATCH_EXECUTABLE patch
      "C:/Program Files/GnuWin32/bin"
      "C:/Program Files (x86)/GnuWin32/bin")
    MARK_AS_ADVANCED(CTK_PATCH_EXECUTABLE)
    IF(NOT CTK_PATCH_EXECUTABLE)
      MESSAGE(FATAL_ERROR "error: Patch is required to build ${PROJECT_NAME}. Set CTK_PATCH_EXECUTABLE")
    ENDIF()

    # Configure patch script
    SET(pythonqt_src_dir ${ep_source_dir}/${proj})
    SET(pythonqt_patch_dir ${CTK_SOURCE_DIR}/Utilities/PythonQt/)
    SET(pythonqt_configured_patch_dir ${CTK_BINARY_DIR}/Utilities/PythonQt/)
    SET(pythonqt_patchscript
      ${CTK_BINARY_DIR}/Utilities/PythonQt/PythonQt-trunk-patch.cmake)
    CONFIGURE_FILE(
      ${CTK_SOURCE_DIR}/Utilities/PythonQt/PythonQt-trunk-patch.cmake.in
      ${pythonqt_patchscript} @ONLY)
      
    ExternalProject_Add(${proj}
      SVN_REPOSITORY "http://pythonqt.svn.sourceforge.net/svnroot/pythonqt/trunk"
      CMAKE_GENERATOR ${gen}
      PATCH_COMMAND ${CMAKE_COMMAND} -P ${pythonqt_patchscript}
      BUILD_COMMAND ""
      CMAKE_ARGS
        ${ep_common_args}
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}
        -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
        -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
        -DPythonQt_Wrap_QtGui:BOOL=ON
      )
    SET(PYTHONQT_INSTALL_DIR ${ep_install_dir})
  ENDIF()
ENDIF()
