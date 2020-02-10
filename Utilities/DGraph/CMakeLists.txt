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

cmake_minimum_required(VERSION 2.8)

foreach(policy CMP0003)
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()

project(DGraph)

set(CTK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CTK_SOURCE_DIR}/Libs/Core
  )

# Do not export symbol in ctkDependencyGraph class
add_definitions(-DNO_SYMBOL_EXPORT)

# Configure CTKCoreExport.h
set(MY_EXPORT_HEADER_PREFIX CTKCore)
set(MY_LIBRARY_EXPORT_DIRECTIVE CTK_CORE_EXPORT)
configure_file(
  ${CTK_SOURCE_DIR}/Libs/ctkExport.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
  )

# Add excutable
add_executable(${PROJECT_NAME}
  DGraph.cpp
  ${CTK_SOURCE_DIR}/Libs/Core/ctkDependencyGraph.h
  ${CTK_SOURCE_DIR}/Libs/Core/ctkDependencyGraph.cpp)
