###########################################################################
#
#  Library: CTK
#
#  Copyright (c) German Cancer Research Center,
#    Division of Medical and Biological Informatics
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#! \ingroup CMakeUtilities
function(ctkFunctionGetIncludeDirs var_include_dirs)

  if(NOT ARGN)
    message(FATAL_ERROR "No targets given")
  endif()

  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

  set(_include_dirs ${${var_include_dirs}} ${CTK_CONFIG_H_INCLUDE_DIR})
  foreach(_target ${ARGN})

    # Add the include directories from the plugin dependencies
    # The variable ${_target}_DEPENDENCIES is set in the
    # macro ctkMacroValidateBuildOptions

    set(ctk_deps )
    set(ext_deps )

    ctkMacroGetAllProjectTargetLibraries("${${_target}_DEPENDENCIES}" ctk_deps)
    ctkMacroGetAllNonProjectTargetLibraries("${${_target}_DEPENDENCIES}" ext_deps)

    foreach(dep ${ctk_deps})

      if(${dep}_INCLUDE_SUFFIXES)
        foreach(_suffix ${${dep}_INCLUDE_SUFFIXES})
          list(APPEND _include_dirs ${${dep}_SOURCE_DIR}/${_suffix})
        endforeach()
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      else()
        list(APPEND _include_dirs
             ${${dep}_SOURCE_DIR}
             ${${dep}_BINARY_DIR}
             )
      endif()

      # Ensure AUTOUIC-generated headers (ui_*.h) are on the include path.
      #
      # By default CMake writes them to:
      #
      #   - Single-config generators (Ninja/Makefiles):
      #       <AUTOGEN_BUILD_DIR>/include
      #
      #   - Multi-config generators (VS, Xcode, Ninja Multi-Config):
      #       <AUTOGEN_BUILD_DIR>/include_<CONFIG>
      #
      # where AUTOGEN_BUILD_DIR defaults to:
      #   <target-binary-dir>/<target-name>_autogen
      #
      # References:
      # - https://cmake.org/cmake/help/latest/manual/cmake-qt.7.html#autouic
      # - https://cmake.org/cmake/help/latest/prop_tgt/AUTOGEN_BUILD_DIR.html
      #
      # Plug-ins are excluded, because that directory also holds AUTOMOC's
      # output, in a subdirectory named after a hash of the source directory
      # relative to the source tree. Plug-ins of a project tend to keep their
      # sources in the same relative directory, so the hash is identical for
      # all of them, and mocs_compilation.cpp includes its moc output with
      # angle brackets. Since CMake appends a target's own autogen include dir
      # last, a plug-in would compile a dependency's meta object instead of its
      # own whenever both have a header of the same name. A plug-in has no
      # reason to include another plug-in's generated ui_*.h.
      #
      # ${dep}_INCLUDE_SUFFIXES is only ever set by ctkMacroBuildPlugin.
      if(NOT DEFINED ${dep}_INCLUDE_SUFFIXES)
        list(APPEND _include_dirs
          ${${dep}_BINARY_DIR}/${dep}_autogen/include$<$<BOOL:${_isMultiConfig}>:_$<CONFIG>>
          )
      endif()

      # For external projects, CTKConfig.cmake contains variables
      # listening the include dirs for CTK libraries and plugins
      if(${dep}_INCLUDE_DIRS)
        list(APPEND _include_dirs ${${dep}_INCLUDE_DIRS})
      endif()
    endforeach()

    foreach(dep ${ext_deps})

      if(${dep}_INCLUDE_DIRS)
        string(REPLACE "^" ";" _ext_include_dirs "${${dep}_INCLUDE_DIRS}")
        list(APPEND _include_dirs ${_ext_include_dirs})
      endif()

      # This is for resolving include dependencies between
      # libraries / plugins from external projects using CTK
      if(${dep}_SOURCE_DIR AND ${dep}_INCLUDE_SUFFIXES)
        foreach(_suffix ${${dep}_INCLUDE_SUFFIXES})
          list(APPEND _include_dirs ${${dep}_SOURCE_DIR}/${_suffix})
        endforeach()
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      elseif(${dep}_SOURCE_DIR)
        list(APPEND _include_dirs ${${dep}_SOURCE_DIR})
      endif()

      if(${dep}_BINARY_DIR)
        list(APPEND _include_dirs ${${dep}_BINARY_DIR})
      endif()

    endforeach()

  endforeach()

  if(_include_dirs)
    list(REMOVE_DUPLICATES _include_dirs)
  endif()
  set(${var_include_dirs} ${_include_dirs} PARENT_SCOPE)

endfunction()

#
# cmake -DTEST_ctk_function_get_include_dirs_test:BOOL=ON -P ctkFunctionGetIncludeDirs.cmake
#
function(ctk_function_get_include_dirs_test)

  include(${CMAKE_CURRENT_LIST_DIR}/ctkMacroParseArguments.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/ctkMacroListFilter.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/ctkMacroTargetLibraries.cmake)

  function(ctk_function_get_include_dirs_test_check id dirs pattern expected)
    set(_found 0)
    foreach(_dir IN LISTS dirs)
      if(_dir MATCHES "${pattern}")
        set(_found 1)
      endif()
    endforeach()
    if(NOT _found EQUAL expected)
      message(FATAL_ERROR "Problem with ctkFunctionGetIncludeDirs() - See testcase: ${id}\n"
                          "pattern:${pattern}\n"
                          "found:${_found}\n"
                          "expected:${expected}\n"
                          "dirs:${dirs}")
    endif()
  endfunction()

  # A library and a plug-in dependency. Only ctkMacroBuildPlugin defines
  # <target>_INCLUDE_SUFFIXES, so that is what tells the two apart.
  set(CTKFoo_SOURCE_DIR "/src/Libs/Foo")
  set(CTKFoo_BINARY_DIR "/bin/Libs/Foo")

  set(org_commontk_bar_SOURCE_DIR "/src/Plugins/org.commontk.bar")
  set(org_commontk_bar_BINARY_DIR "/bin/Plugins/org.commontk.bar")
  set(org_commontk_bar_INCLUDE_SUFFIXES "")

  set(mytarget_DEPENDENCIES CTKFoo org_commontk_bar)

  set(dirs )
  ctkFunctionGetIncludeDirs(dirs mytarget)

  # A library contributes its autogen dir, because a dependent may include the
  # ui_*.h generated for it.
  ctk_function_get_include_dirs_test_check(1 "${dirs}" "CTKFoo_autogen" 1)

  # A plug-in does not. That directory also holds AUTOMOC output in a
  # subdirectory named after a hash of the source directory relative to the
  # source tree, which collides between plug-ins sharing that layout.
  ctk_function_get_include_dirs_test_check(2 "${dirs}" "org_commontk_bar_autogen" 0)

  # The plug-in is still on the include path, only its autogen dir is not.
  ctk_function_get_include_dirs_test_check(3 "${dirs}" "org.commontk.bar" 1)

  message("SUCCESS")
endfunction()
if(TEST_ctk_function_get_include_dirs_test)
  ctk_function_get_include_dirs_test()
endif()
