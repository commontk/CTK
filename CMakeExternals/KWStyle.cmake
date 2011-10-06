#
# KWStyle
#
SET(kwstyle_DEPENDS)
IF(CTK_USE_KWSTYLE)

  # Sanity checks
  IF(DEFINED CTK_KWSTYLE_EXECUTABLE AND NOT EXISTS ${CTK_KWSTYLE_EXECUTABLE})
    MESSAGE(FATAL_ERROR "CTK_KWSTYLE_EXECUTABLE variable is defined but corresponds to non-existing executable")
  ENDIF()

  SET(proj KWStyle-CVSHEAD)
  SET(proj_DEPENDENCIES)

  SET(kwstyle_DEPENDS ${proj})

  IF(NOT DEFINED CTK_KWSTYLE_EXECUTABLE)
    # Set CMake OSX variable to pass down the external project
    set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
    if(APPLE)
      list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
        -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
    endif()

    ExternalProject_Add(${proj}
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      LIST_SEPARATOR ${sep}
      CVS_REPOSITORY ":pserver:anoncvs:@public.kitware.com:/cvsroot/KWStyle"
      CVS_MODULE "KWStyle"
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
        -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
        -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
        ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
        -DBUILD_TESTING:BOOL=OFF
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)

    # Since KWStyle is an executable, there is not need to add its corresponding
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS KWSTYLE_EXECUTABLE:PATH)

ENDIF()
