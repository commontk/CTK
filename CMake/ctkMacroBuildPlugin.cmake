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
MACRO(ctkMacroBuildPlugin)
  CtkMacroParseArguments(MY
    "EXPORT_DIRECTIVE;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;EXPORTED_INCLUDE_SUFFIXES;TARGET_LIBRARIES;RESOURCES;CACHED_RESOURCEFILES;TRANSLATIONS;OUTPUT_DIR"
    "TEST_PLUGIN"
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_EXPORT_DIRECTIVE)
    MESSAGE(FATAL_ERROR "EXPORT_DIRECTIVE is mandatory")
  ENDIF()

  # Plugin are expected to be shared library
  SET(MY_LIBRARY_TYPE "SHARED")

  # Define library name
  SET(lib_name ${PROJECT_NAME})

  # Clear the variables for the manifest headers
  SET(Plugin-ActivationPolicy )
  SET(Plugin-Category )
  SET(Plugin-ContactAddress )
  SET(Plugin-Copyright )
  SET(Plugin-Description )
  SET(Plugin-DocURL )
  SET(Plugin-Icon )
  SET(Plugin-License )
  SET(Plugin-Name )
  SET(Require-Plugin )
  SET(Plugin-SymbolicName )
  SET(Plugin-Vendor )
  SET(Plugin-Version )

  SET(Custom-Headers )

  IF(MY_TEST_PLUGIN)
    # Since the test plug-ins are not considered when calculating
    # target dependencies via DGraph, we add the dependencies
    # manually here
    #MESSAGE("${lib_name}_DEPENDENCIES ${MY_TARGET_LIBRARIES}")
    LIST(APPEND ${lib_name}_DEPENDENCIES ${MY_TARGET_LIBRARIES})
  ENDIF()

  # If a file named manifest_headers.cmake exists, read it
  SET(manifest_headers_dep )
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake)
    SET(manifest_headers_dep "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
  ENDIF()

  STRING(REPLACE "_" "." Plugin-SymbolicName ${lib_name})

  # --------------------------------------------------------------------------
  # Include dirs
  IF(MY_EXPORTED_INCLUDE_SUFFIXES)
    SET(${lib_name}_INCLUDE_SUFFIXES ${MY_EXPORTED_INCLUDE_SUFFIXES}
        CACHE INTERNAL "List of exported plugin include dirs")

    SET(my_includes )
    FOREACH(_suffix ${MY_EXPORTED_INCLUDE_SUFFIXES})
      LIST(APPEND my_includes ${CMAKE_CURRENT_SOURCE_DIR}/${_suffix})
    ENDFOREACH()
  ELSE()
    SET(${lib_name}_INCLUDE_SUFFIXES ""
        CACHE INTERNAL "List of exported plugin include dirs")

    SET(my_includes )
  ENDIF()

  LIST(APPEND my_includes
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${MY_INCLUDE_DIRECTORIES}
      )

  # Add the include directories from the plugin dependencies
  # and external dependencies
  ctkFunctionGetIncludeDirs(my_includes ${lib_name})

  INCLUDE_DIRECTORIES(
    ${my_includes}
    )

  # Add Qt include dirs and defines
  INCLUDE(${QT_USE_FILE})

  # Add the library directories from the external project
  ctkFunctionGetLibraryDirs(my_library_dirs ${lib_name})

  LINK_DIRECTORIES(
    ${my_library_dirs}
    )

  SET(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  SET(MY_EXPORT_HEADER_PREFIX "${lib_name}_")
  SET(MY_LIBNAME ${lib_name})

  CONFIGURE_FILE(
    ${CTK_EXPORT_HEADER_TEMPLATE}
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  SET(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  SET(MY_MOC_CPP)
  SET(MY_UI_CPP)
  SET(MY_QRC_SRCS)

  # Wrap
  IF(MY_MOC_SRCS)
    # this is a workaround for Visual Studio. The relative include paths in the generated
    # moc files can get very long and can't be resolved by the MSVC compiler.
    FOREACH(moc_src ${MY_MOC_SRCS})
      QT4_WRAP_CPP(MY_MOC_CPP ${moc_src} OPTIONS -f${moc_src})
    ENDFOREACH()
  ENDIF()
  QT4_WRAP_UI(MY_UI_CPP ${MY_UI_FORMS})
  IF(DEFINED MY_RESOURCES)
    QT4_ADD_RESOURCES(MY_QRC_SRCS ${MY_RESOURCES})
  ENDIF()

  # Add the generated manifest qrc file
  SET(manifest_qrc_src )
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

  IF(manifest_headers_dep)
    SET_PROPERTY(SOURCE ${manifest_qrc_src} APPEND
                   PROPERTY OBJECT_DEPENDS ${manifest_headers_dep})
  ENDIF()
  LIST(APPEND MY_QRC_SRCS ${manifest_qrc_src})

  # Create translation files (.ts and .qm)
  SET(_plugin_qm_files )
  SET(_plugin_cached_resources_in_binary_tree )
  SET(_translations_dir "${CMAKE_CURRENT_BINARY_DIR}/CTK-INF/l10n")
  IF(MY_TRANSLATIONS)
    SET_SOURCE_FILES_PROPERTIES(${MY_TRANSLATIONS}
                                PROPERTIES OUTPUT_LOCATION ${_translations_dir})
    QT4_CREATE_TRANSLATION(_plugin_qm_files ${MY_SRCS} ${MY_UI_FORMS} ${MY_TRANSLATIONS})
  ENDIF()

  IF(_plugin_qm_files)
    FOREACH(_qm_file ${_plugin_qm_files})
      FILE(RELATIVE_PATH _relative_qm_file ${CMAKE_CURRENT_BINARY_DIR} ${_qm_file})
      LIST(APPEND _plugin_cached_resources_in_binary_tree ${_relative_qm_file})
    ENDFOREACH()
  ENDIF()

  SET(_plugin_cached_resources_in_source_tree )
  IF(MY_CACHED_RESOURCEFILES)
    FOREACH(_cached_resource ${MY_CACHED_RESOURCEFILES})
      IF(IS_ABSOLUTE "${_cached_resource}")
        # create a path relative to the current binary dir
        FILE(RELATIVE_PATH _relative_cached_resource ${CMAKE_CURRENT_BINARY_DIR} ${_cached_resource})
        LIST(APPEND _plugin_cached_resources_in_binary_tree ${_relative_cached_resource})
      ELSE()
        LIST(APPEND _plugin_cached_resources_in_source_tree ${_cached_resource})
      ENDIF()
    ENDFOREACH()
  ENDIF()

  # Add any other additional resource files
  IF(_plugin_cached_resources_in_source_tree OR _plugin_cached_resources_in_binary_tree)
    STRING(REPLACE "." "_" _plugin_symbolicname ${Plugin-SymbolicName})
    ctkMacroGeneratePluginResourceFile(MY_QRC_SRCS
      NAME ${_plugin_symbolicname}_cached.qrc
      PREFIX ${Plugin-SymbolicName}
      RESOURCES ${_plugin_cached_resources_in_source_tree}
      BINARY_RESOURCES ${_plugin_cached_resources_in_binary_tree})
  ENDIF()

  SOURCE_GROUP("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    ${MY_TRANSLATIONS}
    )

  SOURCE_GROUP("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${_plugin_qm_files}
    )

  ADD_LIBRARY(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CPP}
    ${MY_UI_CPP}
    ${MY_QRC_SRCS}
    ${_plugin_qm_files}
    )

  # Set the output directory for the plugin
  IF(MY_OUTPUT_DIR)
    SET(output_dir_suffix "/${MY_OUTPUT_DIR}")
  ELSE()
    SET(output_dir_suffix "")
  ENDIF()

  FOREACH(type RUNTIME LIBRARY ARCHIVE)
    IF(NOT DEFINED CTK_PLUGIN_${type}_OUTPUT_DIRECTORY AND CMAKE_${type}_OUTPUT_DIRECTORY)
      # Put plug-ins by default into a "plugins" subdirectory
      SET(CTK_PLUGIN_${type}_OUTPUT_DIRECTORY "${CMAKE_${type}_OUTPUT_DIRECTORY}/plugins")
    ENDIF()
    
    IF(IS_ABSOLUTE "${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}")
      SET(plugin_${type}_output_dir "${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    ELSEIF(CMAKE_${type}_OUTPUT_DIRECTORY)
      SET(plugin_${type}_output_dir "${CMAKE_${type}_OUTPUT_DIRECTORY}/${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    ELSE()
      SET(plugin_${type}_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${CTK_PLUGIN_${type}_OUTPUT_DIRECTORY}${output_dir_suffix}")
    ENDIF()

    IF(MY_TEST_PLUGIN)
      # Test plug-ins will always be put in a separate directory
      IF(CMAKE_${type}_OUTPUT_DIRECTORY)
        SET(plugin_${type}_output_dir "${CMAKE_${type}_OUTPUT_DIRECTORY}/test_plugins")
      ELSE()
        SET(plugin_${type}_output_dir "${PROJECT_BINARY_DIR}/test_plugins")
      ENDIF()
    ENDIF()
  ENDFOREACH()

  SET(plugin_compile_flags "-DQT_PLUGIN")

  # MinGW does not export all symbols automatically, so no need to set flags.
  #
  # With gcc < 4.5, RTTI symbols from classes declared in third-party libraries
  # which are not "gcc visibility aware" are marked with hidden visibility in
  # DSOs which include the class declaration and which are compiled with
  # hidden visibility. This leads to dynamic_cast and exception handling problems.
  # While this problem could be worked around by sandwiching the include
  # directives for the third-party headers between "#pragma visibility push/pop"
  # statements, it is generally safer to just use default visibility with
  # gcc < 4.5.
  IF(CMAKE_COMPILER_IS_GNUCXX AND NOT ${GCC_VERSION} VERSION_LESS "4.5" AND NOT MINGW)
    SET(plugin_compile_flags "${plugin_compile_flags} -fvisibility=hidden -fvisibility-inlines-hidden")
  ENDIF()

  # Apply properties to the library target.
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES
    COMPILE_FLAGS "${plugin_compile_flags}"
    RUNTIME_OUTPUT_DIRECTORY ${plugin_RUNTIME_output_dir}
    LIBRARY_OUTPUT_DIRECTORY ${plugin_LIBRARY_output_dir}
    ARCHIVE_OUTPUT_DIRECTORY ${plugin_ARCHIVE_output_dir}
    PREFIX "lib"
    )

  # Note: The plugin may be installed in some other location ???
  # Install rules
# IF(MY_LIBRARY_TYPE STREQUAL "SHARED")
# INSTALL(TARGETS ${lib_name}
# RUNTIME DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
# LIBRARY DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
# ARCHIVE DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Development)
# ENDIF()

  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    )

  IF(MINGW)
    LIST(APPEND my_libs ssp) # add stack smash protection lib
  ENDIF()

  TARGET_LINK_LIBRARIES(${lib_name} ${my_libs})

  # Update CTK_PLUGINS
  IF(NOT MY_TEST_PLUGIN)
    SET(CTK_PLUGIN_LIBRARIES ${CTK_PLUGIN_LIBRARIES} ${lib_name} CACHE INTERNAL "CTK plugins" FORCE)
  ENDIF()

  # Install headers
  #FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  #INSTALL(FILES
  # ${headers}
  # ${dynamicHeaders}
  # DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
  # )

ENDMACRO()

