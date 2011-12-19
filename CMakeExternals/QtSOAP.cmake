#
# QtSOAP
#

ctkMacroShouldAddExternalproject(QtSOAP_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED QtSOAP_DIR AND NOT EXISTS ${QtSOAP_DIR})
    message(FATAL_ERROR "QtSOAP_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(QtSOAP_enabling_variable QtSOAP_LIBRARIES)

  set(proj QtSOAP)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  set(${QtSOAP_enabling_variable}_LIBRARY_DIRS QtSOAP_LIBRARY_DIRS)
  set(${QtSOAP_enabling_variable}_INCLUDE_DIRS QtSOAP_INCLUDE_DIRS)
  set(${QtSOAP_enabling_variable}_FIND_PACKAGE_CMD QtSOAP)

  if(CTK_SUPERBUILD)

    if(NOT DEFINED QtSOAP_DIR)

      set(revision_tag 6bf1b8c8)
      if(${proj}_REVISION_TAG)
        set(revision_tag ${${proj}_REVISION_TAG})
      endif()

      # Set CMake OSX variable to pass down the external project
      set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
      if(APPLE)
        list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
          -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
          -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
          -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET})
      endif()

      #     message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        GIT_REPOSITORY "${git_protocol}://github.com/commontk/QtSOAP.git"
        GIT_TAG ${revision_tag}
        CMAKE_GENERATOR ${gen}
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_CACHE_ARGS
          -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
          -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
          -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
          -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY}
          -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
          ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(QtSOAP_DIR "${CMAKE_BINARY_DIR}/${proj}-build")

      # Since the QtSOAP dll is created directly in CTK-build/bin, there is not need to add a
      # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS QtSOAP_DIR:PATH)

  endif()

endif()
