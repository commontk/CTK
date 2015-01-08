###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#  CTK/CMake/ctkMacroGeneratePluginManifest.cmake
#
#! \brief Build a CTK plug-in.
#!
#! This macro takes the usual arguments for building
#! a shared library using Qt. Additionally, it generates
#! plugin meta-data by creating a MANIFEST.MF text file
#! which is embedded in the share library as a Qt resource.
#!
#! The following variables can be set in a file named
#! manifest_headers.cmake, which will then be read by
#! this macro:
#!
#! - Plugin-ActivationPolicy
#! - Plugin-Category
#! - Plugin-ContactAddress
#! - Plugin-Copyright
#! - Plugin-Description
#! - Plugin-DocURL
#! - Plugin-Icon
#! - Plugin-License
#! - Plugin-Name
#! - Require-Plugin
#! - Plugin-Vendor
#! - Plugin-Version
#!
#! \ingroup CMakeAPI
macro(ctkMacroBuildPlugin)
  CtkMacroParseArguments(MY
    "EXPORT_DIRECTIVE;SRCS;MOC_SRCS;MOC_OPTIONS;UI_FORMS;INCLUDE_DIRECTORIES;EXPORTED_INCLUDE_SUFFIXES;TARGET_LIBRARIES;RESOURCES;CACHED_RESOURCEFILES;TRANSLATIONS;OUTPUT_DIR"
    "TEST_PLUGIN;NO_INSTALL"
    ${ARGN}
    )

  # Keep parameter 'INCLUDE_DIRECTORIES' for backward compatiblity

  # Sanity checks
  if(NOT DEFINED MY_EXPORT_DIRECTIVE)
    message(FATAL_ERROR "EXPORT_DIRECTIVE is mandatory")
  endif()

  # Print a warning if the project name does not match the directory name
  get_filename_component(_dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  string(REPLACE "." "_" _dir_name_with_ ${_dir_name})
  if(NOT _dir_name_with_ STREQUAL ${PROJECT_NAME})
    message(WARNING "Discouraged mismatch of plug-in project name [${PROJECT_NAME}] and top-level directory name [${CMAKE_CURRENT_SOURCE_DIR}].")
  endif()

  # Define library name
  set(lib_name ${PROJECT_NAME})

  # Plug-in target names must contain at leas one _
  if(NOT lib_name MATCHES _)
    message(FATAL_ERROR "The plug-in project name ${lib_name} must contain at least one '_' character")
  endif()

  # Plugin are expected to be shared library
  set(MY_LIBRARY_TYPE "SHARED")

  # Clear the variables for the manifest headers
  set(Plugin-ActivationPolicy )
  set(Plugin-Category )
  set(Plugin-ContactAddress )
  set(Plugin-Copyright )
  set(Plugin-Description )
  set(Plugin-DocURL )
  set(Plugin-Icon )
  set(Plugin-License )
  set(Plugin-Name )
  set(Require-Plugin )
  set(Plugin-SymbolicName )
  set(Plugin-Vendor )
  set(Plugin-Version )

  set(Custom-Headers )

  if(MY_TEST_PLUGIN)
    # Since the test plug-ins are not considered when calculating
    # target dependencies via DGraph, we add the dependencies
    # manually here
    #message("${lib_name}_DEPENDENCIES ${MY_TARGET_LIBRARIES}")
    list(APPEND ${lib_name}_DEPENDENCIES ${MY_TARGET_LIBRARIES})
  endif()

  # If a file named manifest_headers.cmake exists, read it
  set(manifest_headers_dep )
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
    include(${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake)
    set(manifest_headers_dep "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
  endif()

  string(REPLACE "_" "." Plugin-SymbolicName ${lib_name})

  # --------------------------------------------------------------------------
  # Include dirs
  if(MY_EXPORTED_INCLUDE_SUFFIXES)
    set(${lib_name}_INCLUDE_SUFFIXES ${MY_EXPORTED_INCLUDE_SUFFIXES}
        CACHE INTERNAL "List of exported plugin include dirs")

    set(my_includes )
    foreach(_suffix ${MY_EXPORTED_INCLUDE_SUFFIXES})
      list(APPEND my_includes ${CMAKE_CURRENT_SOURCE_DIR}/${_suffix})
    endforeach()
  else()
    set(${lib_name}_INCLUDE_SUFFIXES ""
        CACHE INTERNAL "List of exported plugin include dirs")

    set(my_includes )
  endif()

  list(APPEND my_includes
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      )

  # Add the include directories from the plugin dependencies
  # and external dependencies
  ctkFunctionGetIncludeDirs(my_includes ${lib_name})

  if(CMAKE_VERSION VERSION_LESS 2.8.12)
    include_directories(
      ${my_includes}
      )
  endif()

  if(CTK_QT_VERSION VERSION_LESS "5")
    # Add Qt include dirs and defines
    include(${QT_USE_FILE})
  else()
    find_package(Qt5LinguistTools REQUIRED)
  endif()

  # Add the library directories from the external project
  ctkFunctionGetLibraryDirs(my_library_dirs ${lib_name})

  link_directories(
    ${my_library_dirs}
    )

  set(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  set(MY_EXPORT_HEADER_PREFIX "${lib_name}_")
  set(MY_LIBNAME ${lib_name})

  configure_file(
    ${CTK_EXPORT_HEADER_TEMPLATE}
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  set(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  set(MY_MOC_CPP)
  set(MY_UI_CPP)
  set(MY_QRC_SRCS)

  # Wrap
  if (CTK_QT_VERSION VERSION_GREATER "4")
    if(MY_MOC_SRCS)
      # this is a workaround for Visual Studio. The relative include paths in the generated
      # moc files can get very long and can't be resolved by the MSVC compiler.
      foreach(moc_src ${MY_MOC_SRCS})
        QT5_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src} -DHAVE_QT5 TARGET ${lib_name})
      endforeach()
    endif()
    QT5_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
    if(DEFINED MY_RESOURCES)
      QT5_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
    endif()
  else()
    if(MY_MOC_SRCS)
      # this is a workaround for Visual Studio. The relative include paths in the generated
      # moc files can get very long and can't be resolved by the MSVC compiler.
      foreach(moc_src ${MY_MOC_SRCS})
        QT4_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src} TARGET ${lib_name})
      endforeach()
    endif()
    QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
    if(DEFINED MY_RESOURCES)
      QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
    endif()
  endif()
  # Add the generated manifest qrc file
  set(manifest_qrc_src )
  ctkFunctionGeneratePluginManifest(manifest_qrc_src
    ACTIVATIONPOLICY ${Plugin-ActivationPolicy}
    CATEGORY ${Plugin-Category}
    CONTACT_ADDRESS ${Plugin-ContactAddress}
    COPYRIGHT ${Plugin-Copyright}
    DESCRIPTION ${Plugin-Description}
    DOC_URL ${Plugin-DocURL}
    ICON ${Plugin-Icon}
    LICENSE ${Plugin-License}
    NAME ${Plugin-Name}
    REQUIRE_PLUGIN ${Require-Plugin}
    SYMBOLIC_NAME ${Plugin-SymbolicName}
    VENDOR ${Plugin-Vendor}
    VERSION ${Plugin-Version}
    CUSTOM_HEADERS ${Custom-Headers}
    )

  if(manifest_headers_dep)
    set_property(SOURCE ${manifest_qrc_src} APPEND
                   PROPERTY OBJECT_DEPENDS ${manifest_headers_dep})
  endif()
  list(APPEND MY_QRC_SRCS ${manifest_qrc_src})

  # Create translation files (.ts and .qm)
  set(_plugin_qm_files )
  set(_plugin_cached_resources_in_binary_tree )
  set(_translations_dir "${CMAKE_CURRENT_BINARY_DIR}/CTK-INF/l10n")
  if(MY_TRANSLATIONS)
    set_source_files_properties(${MY_TRANSLATIONS}
                                PROPERTIES OUTPUT_LOCATION ${_translations_dir})
    if(CTK_QT_VERSION VERSION_GREATER "4")
      qt5_create_translation(_plugin_qm_files ${MY_SRCS} ${MY_UI_FORMS} ${MY_TRANSLATIONS})
    else()
      QT4_CREATE_TRANSLATION(_plugin_qm_files ${MY_SRCS} ${MY_UI_FORMS} ${MY_TRANSLATIONS})
    endif()
  endif()

  if(_plugin_qm_files)
    foreach(_qm_file ${_plugin_qm_files})
      file(RELATIVE_PATH _relative_qm_file ${CMAKE_CURRENT_BINARY_DIR} ${_qm_file})
      list(APPEND _plugin_cached_resources_in_binary_tree ${_relative_qm_file})
    endforeach()
  endif()

  set(_plugin_cached_resources_in_source_tree )
  if(MY_CACHED_RESOURCEFILES)
    foreach(_cached_resource ${MY_CACHED_RESOURCEFILES})
      if(IS_ABSOLUTE "${_cached_resource}")
        # create a path relative to the current binary dir
        file(RELATIVE_PATH _relative_cached_resource ${CMAKE_CURRENT_BINARY_DIR} ${_cached_resource})
        list(APPEND _plugin_cached_resources_in_binary_tree ${_relative_cached_resource})
      else()
        list(APPEND _plugin_cached_resources_in_source_tree ${_cached_resource})
      endif()
    endforeach()
  endif()

  # Add any other additional resource files
  if(_plugin_cached_resources_in_source_tree OR _plugin_cached_resources_in_binary_tree)
    string(REPLACE "." "_" _plugin_symbolicname ${Plugin-SymbolicName})
    ctkMacroGeneratePluginResourcefile(MY_QRC_SRCS
      NAME ${_plugin_symbolicname}_cached.qrc
      PREFIX ${Plugin-SymbolicName}
      RESOURCES ${_plugin_cached_resources_in_source_tree}
      BINARY_RESOURCES ${_plugin_cached_resources_in_binary_tree})
  endif()

  source_group("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    ${MY_TRANSLATIONS}
    )

  source_group("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${_plugin_qm_files}
    )

  add_library(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    ${_plugin_qm_files}
    )

  if(NOT CMAKE_VERSION VERSION_LESS 2.8.12)
    target_include_directories(${lib_name}
      PUBLIC "$<BUILD_INTERFACE:${my_includes}>"
             "$<INSTALL_INTERFACE:${CTK_INSTALL_PLUGIN_INCLUDE_DIR}/${Plugin-SymbolicName}>"
      )
    if(CTK_QT_VERSION VERSION_LESS "5")
      # Add Qt include dirs to the target
      target_include_directories(${lib_name} PUBLIC ${QT_INCLUDE_DIR})
      foreach(module QT3SUPPORT QTOPENGL QTASSISTANT QTDESIGNER QTMOTIF QTNSPLUGIN
               QAXSERVER QAXCONTAINER QTDECLARATIVE QTSCRIPT QTSVG QTUITOOLS QTHELP
               QTWEBKIT PHONON QTSCRIPTTOOLS QTMULTIMEDIA QTXMLPATTERNS QTGUI QTTEST
               QTDBUS QTXML QTSQL QTNETWORK QTCORE)
        if (QT_USE_${module} OR QT_USE_${module}_DEPENDS)
          if (QT_${module}_FOUND)
            target_include_directories(${lib_name} PUBLIC ${QT_${module}_INCLUDE_DIR})
          endif ()
        endif ()
      endforeach()
    endif()
  else()
    find_package(Qt5LinguistTools REQUIRED)
  endif()

  if(MY_TEST_PLUGIN AND CTK_QT_VERSION VERSION_GREATER "4")
    find_package(Qt5Test REQUIRED)
    if(CMAKE_VERSION VERSION_LESS 2.8.12)
      target_link_libraries(${lib_name} Qt5::Test)
    else()
      target_link_libraries(${lib_name} PRIVATE Qt5::Test)
    endif()
  endif()

  # Set the output directory for the plugin
  if(MY_OUTPUT_DIR)
    set(output_dir_suffix "/${MY_OUTPUT_DIR}")
  else()
    set(output_dir_suffix "")
  endif()

  foreach(type RUNTIME LIBRARY ARCHIVE)
    if(NOT DEFINED CTK_PLUGIN_${type}_OUTPUT_DIRECTORY AND CMAKE_${type}_OUTPUT_DIRECTORY)
      # Put plug-ins by default into a "plugins" subdirectory
      set(CTK_PLUGIN_${type}_OUTPUT_DIRECTORY "${CMAKE_${type}_OUTPUT_DIRECTORY}/plugins")
    endif()

    if(IS_ABSOLUTE "${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}")
      set(plugin_${type}_output_dir "${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    elseif(CMAKE_${type}_OUTPUT_DIRECTORY)
      set(plugin_${type}_output_dir "${CMAKE_${type}_OUTPUT_DIRECTORY}/${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    else()
      set(plugin_${type}_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    endif()

    if(MY_TEST_PLUGIN)
      # Test plug-ins will always be put in a separate directory
      if(CMAKE_${type}_OUTPUT_DIRECTORY)
        set(plugin_${type}_output_dir "${CMAKE_${type}_OUTPUT_DIRECTORY}/test_plugins")
      else()
        set(plugin_${type}_output_dir "${PROJECT_BINARY_DIR}/test_plugins")
      endif()
    endif()
  endforeach()

  set(plugin_compile_flags "-DQT_PLUGIN")
  ctkFunctionGetCompilerVisibilityFlags(plugin_compile_flags)

  # Apply properties to the library target.
  set_target_properties(${lib_name} PROPERTIES
    COMPILE_FLAGS "${plugin_compile_flags}"
    RUNTIME_OUTPUT_DIRECTORY ${plugin_RUNTIME_output_dir}
    LIBRARY_OUTPUT_DIRECTORY ${plugin_LIBRARY_output_dir}
    ARCHIVE_OUTPUT_DIRECTORY ${plugin_ARCHIVE_output_dir}
    PREFIX "lib"
    )

  if(NOT MY_TEST_PLUGIN AND NOT MY_NO_INSTALL)
    # Install rules
    install(TARGETS ${lib_name} EXPORT CTKExports
      RUNTIME DESTINATION ${CTK_INSTALL_PLUGIN_DIR} COMPONENT RuntimePlugins
      LIBRARY DESTINATION ${CTK_INSTALL_PLUGIN_DIR} COMPONENT RuntimePlugins
      ARCHIVE DESTINATION ${CTK_INSTALL_PLUGIN_DIR} COMPONENT Development)
  endif()

  set(my_libs
    ${MY_TARGET_LIBRARIES}
    )

  if(MINGW)
    list(APPEND my_libs ssp) # add stack smash protection lib
  endif()

  if(CMAKE_VERSION VERSION_LESS 2.8.12)
    target_link_libraries(${lib_name} ${my_libs})
  else()
    target_link_libraries(${lib_name} PUBLIC ${my_libs})
  endif()

  if(NOT MY_TEST_PLUGIN)
    set(${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES ${${CMAKE_PROJECT_NAME}_PLUGIN_LIBRARIES} ${lib_name} CACHE INTERNAL "CTK plugins" FORCE)
  endif()

  if(NOT MY_TEST_PLUGIN AND NOT MY_NO_INSTALL)
    # Install headers
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")
    install(FILES
      ${headers}
      ${dynamicHeaders}
      DESTINATION ${CTK_INSTALL_PLUGIN_INCLUDE_DIR}/${Plugin-SymbolicName} COMPONENT Development
      )
  endif()

endmacro()

