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
  ${CTK_SOURCE_DIR}/Libs/Core
  )

# Add excutable
ADD_EXECUTABLE(${PROJECT_NAME}
  DGraph.cxx
  ${CTK_SOURCE_DIR}/Libs/Core/ctkDependencyGraph.h
  ${CTK_SOURCE_DIR}/Libs/Core/ctkDependencyGraph.cxx)
  
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${QT_LIBRARIES} )

  