#.rst:
# CMakeFindDependencyMacro
# -------------------------
#
# ::
#
#     find_dependency(<dep> [<version> [EXACT]])
#
#
# ``find_dependency()`` wraps a :command:`find_package` call for a package
# dependency. It is designed to be used in a <package>Config.cmake file, and it
# forwards the correct parameters for EXACT, QUIET and REQUIRED which were
# passed to the original :command:`find_package` call.  It also sets an
# informative diagnostic message if the dependency could not be found.
#

#=============================================================================
# Copyright 2013 Stephen Kelly <steveire@gmail.com>
#
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2014 Kitware, Inc.
# Copyright 2000-2011 Insight Software Consortium
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#=============================================================================

macro(find_dependency dep)
  if (NOT ${dep}_FOUND)
    set(cmake_fd_version)
    if (${ARGC} GREATER 1)
      if ("${ARGV1}" STREQUAL "")
        message(FATAL_ERROR "Invalid arguments to find_dependency. VERSION is empty")
      endif()
      if ("${ARGV1}" STREQUAL EXACT)
        message(FATAL_ERROR "Invalid arguments to find_dependency. EXACT may only be specified if a VERSION is specified")
      endif()
      set(cmake_fd_version ${ARGV1})
    endif()
    set(cmake_fd_exact_arg)
    if(${ARGC} GREATER 2)
      if (NOT "${ARGV2}" STREQUAL EXACT)
        message(FATAL_ERROR "Invalid arguments to find_dependency")
      endif()
      set(cmake_fd_exact_arg EXACT)
    endif()
    if(${ARGC} GREATER 3)
      message(FATAL_ERROR "Invalid arguments to find_dependency")
    endif()
    set(cmake_fd_quiet_arg)
    if(${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
      set(cmake_fd_quiet_arg QUIET)
    endif()
    set(cmake_fd_required_arg)
    if(${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
      set(cmake_fd_required_arg REQUIRED)
    endif()

    get_property(cmake_fd_alreadyTransitive GLOBAL PROPERTY
      _CMAKE_${dep}_TRANSITIVE_DEPENDENCY
    )

    find_package(${dep} ${cmake_fd_version}
        ${cmake_fd_exact_arg}
        ${cmake_fd_quiet_arg}
        ${cmake_fd_required_arg}
    )

    if(NOT DEFINED cmake_fd_alreadyTransitive OR cmake_fd_alreadyTransitive)
      set_property(GLOBAL PROPERTY _CMAKE_${dep}_TRANSITIVE_DEPENDENCY TRUE)
    endif()

    if (NOT ${dep}_FOUND)
      set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE "${CMAKE_FIND_PACKAGE_NAME} could not be found because dependency ${dep} could not be found.")
      set(${CMAKE_FIND_PACKAGE_NAME}_FOUND False)
      return()
    endif()
    set(cmake_fd_version)
    set(cmake_fd_required_arg)
    set(cmake_fd_quiet_arg)
    set(cmake_fd_exact_arg)
  endif()
endmacro()
