#
# VTK
#
SET (VTK_DEPENDS)
ctkMacroShouldAddExternalProject(VTK_LIBRARIES add_project)
IF(${add_project} OR CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  # Sanity checks
  IF(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
    MESSAGE(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(VTK_enabling_variable VTK_LIBRARIES)
  
  SET(additional_vtk_cmakevars )
  IF(MINGW)
    LIST(APPEND additional_vtk_cmakevars -DCMAKE_USE_PTHREADS:BOOL=OFF)
  ENDIF()
  
  IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
    LIST(APPEND additional_vtk_cmakevars
      -DPYTHON_EXECUTABLE:PATH=${PYTHON_EXECUTABLE}
      -DPYTHON_LIBRARIES:FILEPATH=${PYTHON_LIBRARIES}
      -DPYTHON_DEBUG_LIBRARIES:FILEPATH=${PYTHON_DEBUG_LIBRARIES}
      )
  ENDIF()

  SET(proj VTK)
  SET(proj_DEPENDENCIES)
  
  SET(VTK_DEPENDS ${proj})
  
  IF(NOT DEFINED VTK_DIR)
#     MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      GIT_REPOSITORY ${git_protocol}://vtk.org/VTK.git
      GIT_TAG "origin/master"
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
        ${additional_vtk_cmakevars}
        -DVTK_WRAP_TCL:BOOL=OFF
        -DVTK_USE_TK:BOOL=OFF
        -DVTK_WRAP_PYTHON:BOOL=${CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK}
        -DVTK_WRAP_JAVA:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=ON 
        -DDESIRED_QT_VERSION:STRING=4
        -DVTK_USE_GUISUPPORT:BOOL=ON
        -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
        -DVTK_USE_QT:BOOL=ON
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(VTK_DIR ${ep_build_dir}/${proj})
    
    # Since the link directories associated with VTK is used, it makes sens to 
    # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
    LIST(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${VTK_DIR}/bin)
    
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  SET(${VTK_enabling_variable}_INCLUDE_DIRS VTK_INCLUDE_DIRS)
  SET(${VTK_enabling_variable}_FIND_PACKAGE_CMD VTK)
ENDIF()

