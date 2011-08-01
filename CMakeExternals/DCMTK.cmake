#
# DCMTK
#

SET(DCMTK_DEPENDS)
ctkMacroShouldAddExternalProject(DCMTK_LIBRARIES add_project)
IF(${add_project})
  
  # Sanity checks
  IF(DEFINED DCMTK_DIR AND NOT EXISTS ${DCMTK_DIR})
    MESSAGE(FATAL_ERROR "DCMTK_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(DCMTK_enabling_variable DCMTK_LIBRARIES)
  
  SET(proj DCMTK)
  SET(proj_DEPENDENCIES)
  
  SET(DCMTK_DEPENDS ${proj})
  
  IF(NOT DEFINED DCMTK_DIR)
#     MESSAGE(STATUS "Adding project:${proj}")
    # Set CMake OSX variable to pass down the external project
    set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
    if(APPLE)
      list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()
    
    #-----------------------------------------------------------------------------
    # HACK Waiting upstream DCMTK is fixed, specify -fPIC here.
    #      See http://public.kitware.com/pipermail/ctk-developers/2011-August/000725.html
    #-----------------------------------------------------------------------------
    set(dcmtk_cxx_flags ${ep_common_cxx_flags})
    set(dcmtk_c_flags ${ep_common_c_flags})
    # To fix compilation problem: relocation R_X86_64_32 against `a local symbol' can not be
    # used when making a shared object; recompile with -fPIC
    # See http://www.cmake.org/pipermail/cmake/2007-May/014350.html
    #
    IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set(dcmtk_cxx_flags "${ep_common_cxx_flags} -fPIC")
      set(dcmtk_c_flags "${ep_common_c_flags} -fPIC")
    ENDIF()

    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      GIT_REPOSITORY "http://git.dcmtk.org/dcmtk.git"
      GIT_TAG "origin/master"
      CMAKE_GENERATOR ${gen}
      BUILD_COMMAND ""
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_FLAGS:STRING=${dcmtk_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${dcmtk_c_flags}
        -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
        ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
        -DBUILD_TESTING:BOOL=OFF
        -DDCMTK_BUILD_APPS:BOOL=ON # Build also dmctk tools (movescu, storescp, ...)
        -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF)
    SET(DCMTK_DIR ${ep_install_dir})

# This was used during heavy development on DCMTK itself.
# Disabling it for now. (It also leads to to build errors
# with the XCode CMake generator on Mac).
#
#    ExternalProject_Add_Step(${proj} force_rebuild
#      COMMENT "Force ${proj} re-build"
#      DEPENDERS build    # Steps that depend on this step
#      ALWAYS 1
#      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${proj}-build
#      DEPENDS
#        ${proj_DEPENDENCIES}
#      )
      
    # Since DCMTK is statically build, there is not need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
  
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
  
  LIST(APPEND CTK_SUPERBUILD_EP_ARGS -DDCMTK_DIR:PATH=${DCMTK_DIR})

  SET(${DCMTK_enabling_variable}_INCLUDE_DIRS DCMTK_INCLUDE_DIR)
  SET(${DCMTK_enabling_variable}_FIND_PACKAGE_CMD DCMTK)
ENDIF()
