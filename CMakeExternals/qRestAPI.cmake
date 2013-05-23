#
# qRestAPI
#
ctkMacroShouldAddExternalproject(qRestAPI_LIBRARIES add_project)
if(${add_project})
  # Sanity checks
  if(DEFINED qRestAPI_DIR AND NOT EXISTS ${qRestAPI_DIR})
    message(FATAL_ERROR "qRestAPI_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(qRestAPI_enabling_variable qRestAPI_LIBRARIES)

  set(proj qRestAPI)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  set(${qRestAPI_enabling_variable}_LIBRARY_DIRS qRestAPI_LIBRARY_DIRS)
  set(${qRestAPI_enabling_variable}_INCLUDE_DIRS qRestAPI_INCLUDE_DIRS)
  set(${qRestAPI_enabling_variable}_FIND_PACKAGE_CMD qRestAPI)

  if(CTK_SUPERBUILD)

    if(NOT DEFINED qRestAPI_DIR)

      set(revision_tag "94a106e2860")
      if(${proj}_REVISION_TAG)
        set(revision_tag ${${proj}_REVISION_TAG})
      endif()
      
      set(location_args )
      if(${proj}_URL)
        set(location_args URL ${${proj}_URL})
      elseif(${proj}_GIT_REPOSITORY)
        set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                          GIT_TAG ${revision_tag})
      else()
        set(location_args GIT_REPOSITORY "${git_protocol}://github.com/commontk/qRestAPI.git"
                          GIT_TAG ${revision_tag})
        #set(location_args URL ....tar.gz)
      endif()
      
      set(ep_project_include_arg)
      #if(CTEST_USE_LAUNCHERS)
      #  set(ep_project_include_arg
      #    "-DCMAKE_PROJECT_qRestAPI_INCLUDE:FILEPATH=${CMAKE_ROOT}/Modules/CTestUseLaunchers.cmake")
      #endif()

      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          ${ep_project_include_arg}
          -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        DEPENDS
          ${proj_DEPENDENCIES}
        )
      set(qRestAPI_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-build)

      # Since the link directories associated with qRestAPI is used, it makes sense to
      # update CTK_EXTERNAL_LIBRARY_DIRS with its associated library output directory
      list(APPEND CTK_EXTERNAL_LIBRARY_DIRS ${qRestAPI_DIR})

    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS qRestAPI_DIR:PATH)

  endif()

endif()

