#
# QtSOAP
#

SET(QtSOAP_DEPENDS)
ctkMacroShouldAddExternalProject(QtSOAP_LIBRARIES add_project)
IF(${add_project})

  # Sanity checks
  IF(DEFINED QtSOAP_DIR AND NOT EXISTS ${QtSOAP_DIR})
    MESSAGE(FATAL_ERROR "QtSOAP_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()

  SET(QtSOAP_enabling_variable QtSOAP_LIBRARIES)

  SET(proj QtSOAP)
  SET(proj_DEPENDENCIES)

  SET(QtSOAP_DEPENDS ${proj})

  IF(NOT DEFINED QtSOAP_DIR)

    SET(revision_tag 6bf1b8c8)
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
      GIT_REPOSITORY "${git_protocol}://github.com/commontk/QtSOAP.git"
      GIT_TAG ${revision_tag}
      CMAKE_GENERATOR ${gen}
      UPDATE_COMMAND ""
      INSTALL_COMMAND ""
      CMAKE_ARGS
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
    SET(QtSOAP_DIR "${CMAKE_BINARY_DIR}/${proj}-build")

    # Since the QtSOAP dll is created directly in CTK-build/bin, there is not need to add a
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()

  LIST(APPEND CTK_SUPERBUILD_EP_VARS QtSOAP_DIR:PATH)

  SET(${QtSOAP_enabling_variable}_LIBRARY_DIRS QtSOAP_LIBRARY_DIRS)
  SET(${QtSOAP_enabling_variable}_INCLUDE_DIRS QtSOAP_INCLUDE_DIRS)
  SET(${QtSOAP_enabling_variable}_FIND_PACKAGE_CMD QtSOAP)

ENDIF()
