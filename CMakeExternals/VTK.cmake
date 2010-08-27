#
# VTK
#
SET (VTK_DEPENDS)
ctkMacroShouldAddExternalProject(VTK_LIBRARIES add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
    MESSAGE(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  SET(additional_vtk_cmakevars )
  IF(MINGW)
    SET(additional_vtk_cmakevars "-DCMAKE_USE_PTHREADS:BOOL=OFF")
  ENDIF()
  
  IF(NOT DEFINED VTK_DIR)
    SET(proj VTK)
#     MESSAGE(STATUS "Adding project:${proj}")
    SET(VTK_DEPENDS ${proj})
    ExternalProject_Add(${proj}
      GIT_REPOSITORY ${git_protocol}://vtk.org/VTK.git
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
        ${additional_vtk_cmakevars}
        -DVTK_WRAP_TCL:BOOL=OFF
        -DVTK_WRAP_PYTHON:BOOL=OFF
        -DVTK_WRAP_JAVA:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=ON 
        -DDESIRED_QT_VERSION:STRING=4
        -DVTK_USE_GUISUPPORT:BOOL=ON
        -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
        -DVTK_USE_QT:BOOL=ON
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      )
    SET(VTK_DIR ${ep_build_dir}/${proj})
    
    # Since the link directories associated with VTK is used, it makes sens to 
    # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
    LIST(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${VTK_DIR}/bin)
  
  ENDIF()
ENDIF()
