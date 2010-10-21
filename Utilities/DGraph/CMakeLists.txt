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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FOREACH(policy CMP0003)
  IF(POLICY ${policy})
    CMAKE_POLICY(SET ${policy} NEW)
  ENDIF()
ENDFOREACH()

PROJECT(DGraph)

SET(CTK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# QT is required
INCLUDE(${CTK_SOURCE_DIR}/CMake/ctkMacroSetupQt.cmake)
ctkMacroSetupQt()

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CTK_SOURCE_DIR}/Libs/Core
  )

# Do not export symbol in ctkDependencyGraph class
ADD_DEFINITIONS(-DNO_SYMBOL_EXPORT)

# Configure CTKCoreExport.h
SET(MY_EXPORT_HEADER_PREFIX CTKCore)
SET(MY_LIBRARY_EXPORT_DIRECTIVE CTK_CORE_EXPORT)
CONFIGURE_FILE(
  ${CTK_SOURCE_DIR}/Libs/ctkExport.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/${MY_EXPORT_HEADER_PREFIX}Export.h
  )

# Add excutable
ADD_EXECUTABLE(${PROJECT_NAME}
  DGraph.cpp
  ${CTK_SOURCE_DIR}/Libs/Core/ctkDependencyGraph.h
  ${CTK_SOURCE_DIR}/Libs/Core/ctkDependencyGraph.cpp)
  
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES} )

  
