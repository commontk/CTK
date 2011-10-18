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

    SET(revision_tag 4b419e67427e974b51ea0812bae25b174426a2ff)
    IF(${proj}_REVISION_TAG)
      SET(revision_tag ${${proj}_REVISION_TAG})
    ENDIF()

    # Set CMake OSX variable to pass down the external project
    set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
    if(APPLE)
      list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()

#     MESSAGE(STATUS "Adding project:${proj}")
    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY ${git_protocol}://vtk.org/VTK.git
      GIT_TAG ${revision_tag}
      UPDATE_COMMAND ""
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
        ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
        -DBUILD_TESTING:BOOL=OFF
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
    SET(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

    # Since the link directories associated with VTK is used, it makes sens to
    # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
    LIST(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${VTK_DIR}/bin)

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS VTK_DIR:PATH)

  SET(${VTK_enabling_variable}_LIBRARY_DIRS VTK_LIBRARY_DIRS)
  SET(${VTK_enabling_variable}_INCLUDE_DIRS VTK_INCLUDE_DIRS)
  SET(${VTK_enabling_variable}_FIND_PACKAGE_CMD VTK)
ENDIF()

