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
#      http://www.commontk.org/LICENSE
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
# This macro takes the usual arguments for building
# a shared library using Qt. Additionally, it generates
# plugin meta-data by creating a MANIFEST.MF text file
# which is embedded in the share library as a Qt resource.
#
# The following variables can be set in a file named
# manifest_headers.cmake, which will then be read by
# this macro:
#
# Plugin-ActivationPolicy
# Plugin-Category
# Plugin-ContactAddress
# Plugin-Copyright
# Plugin-Description
# Plugin-DocURL
# Plugin-Icon
# Plugin-License
# Plugin-Name
# Require-Plugin
# Plugin-Vendor
# Plugin-Version
#
MACRO(ctkMacroBuildPlugin)
  CtkMacroParseArguments(MY
    "EXPORT_DIRECTIVE;SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES;CACHED_RESOURCEFILES;LIBRARY_TYPE"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_EXPORT_DIRECTIVE)
    MESSAGE(SEND_ERROR "EXPORT_DIRECTIVE is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_LIBRARY_TYPE)
    SET(MY_LIBRARY_TYPE "SHARED")
  ENDIF()

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

  # If a file named manifest_headers.cmake exists, read it
  SET(manifest_headers_dep )
  IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake)
    SET(manifest_headers_dep "${CMAKE_CURRENT_SOURCE_DIR}/manifest_headers.cmake")
  ENDIF()

  STRING(REPLACE "_" "." Plugin-SymbolicName ${lib_name})

  # --------------------------------------------------------------------------
  # Include dirs
  SET(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${MY_INCLUDE_DIRECTORIES}
    )

  # Add the include directories from the plugin dependencies
  # The variable ${lib_name}_DEPENDENCIES is set in the
  # macro ctkMacroValidateBuildOptions
  SET(ctk_deps )
  SET(ext_deps )
  ctkMacroGetAllCTKTargetLibraries("${${lib_name}_DEPENDENCIES}" ctk_deps)
  ctkMacroGetAllNonCTKTargetLibraries("${${lib_name}_DEPENDENCIES}" ext_deps)

  FOREACH(dep ${ctk_deps})
    LIST(APPEND my_includes
         ${${dep}_SOURCE_DIR}
         ${${dep}_BINARY_DIR}
         )
  ENDFOREACH()

  FOREACH(dep ${ext_deps})
    STRING(REPLACE "^" ";" _include_dirs "${${dep}_INCLUDE_DIRS}")
    LIST(APPEND my_includes ${_include_dirs})
  ENDFOREACH()

  LIST(REMOVE_DUPLICATES my_includes)

  INCLUDE_DIRECTORIES(
    ${my_includes}
    )
 
  SET(MY_LIBRARY_EXPORT_DIRECTIVE ${MY_EXPORT_DIRECTIVE})
  SET(MY_EXPORT_HEADER_PREFIX "${lib_name}_")
  SET(MY_LIBNAME ${lib_name})
  
  CONFIGURE_FILE(
    ${CTK_SOURCE_DIR}/Libs/CTKExport.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
    )
  SET(dynamicHeaders
    "${dynamicHeaders};${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h")

  # Make sure variable are cleared
  SET(MY_MOC_CXX)
  SET(MY_UI_CXX)
  SET(MY_QRC_SRCS)

  # Wrap
  QT4_WRAP_CPP(MY_MOC_CXX ${MY_MOC_SRCS})
  QT4_WRAP_UI(MY_UI_CXX ${MY_UI_FORMS})
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
    )

  IF(manifest_headers_dep)
    SET_PROPERTY(SOURCE ${manifest_qrc_src} APPEND
                   PROPERTY OBJECT_DEPENDS ${manifest_headers_dep})
  ENDIF()
  LIST(APPEND MY_QRC_SRCS ${manifest_qrc_src})

  # Add any other additional resource files
  IF(MY_CACHED_RESOURCEFILES)
    STRING(REPLACE "." "_" _plugin_symbolicname ${Plugin-SymbolicName})
    ctkMacroGeneratePluginResourceFile(MY_QRC_SRCS
      NAME ${_plugin_symbolicname}_cached.qrc
      PREFIX ${Plugin-SymbolicName}
      RESOURCES ${MY_CACHED_RESOURCEFILES})
  ENDIF()

  SOURCE_GROUP("Resources" FILES
    ${MY_RESOURCES}
    ${MY_UI_FORMS}
    )

  SOURCE_GROUP("Generated" FILES
    ${MY_QRC_SRCS}
    ${MY_MOC_CXX}
    ${MY_UI_CXX}
    )
  
  ADD_LIBRARY(${lib_name} ${MY_LIBRARY_TYPE}
    ${MY_SRCS}
    ${MY_MOC_CXX}
    ${MY_UI_CXX}
    ${MY_QRC_SRCS}
    )

  # Apply properties to the library target.
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES
    COMPILE_FLAGS "-DQT_PLUGIN"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/plugins"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/plugins"
    PREFIX "lib"
    )

  # Note: The plugin may be installed in some other location ???
  # Install rules
# IF(CTK_BUILD_SHARED_LIBS)
# INSTALL(TARGETS ${lib_name}
# RUNTIME DESTINATION ${CTK_INSTALL_BIN_DIR} COMPONENT Runtime
# LIBRARY DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Runtime
# ARCHIVE DESTINATION ${CTK_INSTALL_LIB_DIR} COMPONENT Development)
# ENDIF()
  
  SET(my_libs
    ${MY_TARGET_LIBRARIES}
    )

  IF(MINGW)
    LIST(APPEND my_libs ssp) # add stack smash protection lib
  ENDIF()

  TARGET_LINK_LIBRARIES(${lib_name} ${my_libs})
  
  # Install headers
  #FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  #INSTALL(FILES
  # ${headers}
  # ${dynamicHeaders}
  # DESTINATION ${CTK_INSTALL_INCLUDE_DIR} COMPONENT Development
  # )

ENDMACRO()

