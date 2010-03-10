SET(cmake_version_required "2.8")
SET(cmake_version_required_regex "2.8.*")
SET(cmake_version_required_dash "2-8")

IF(NOT CMAKE_VERSION MATCHES "${cmake_version_required_regex}")
  SET(err "error: CTK SuperBuild requires CMake version ${cmake_version_required}")
  MESSAGE(FATAL_ERROR "${err}")
ENDIF()

CMAKE_MINIMUM_REQUIRED(VERSION ${cmake_version_required})

# 
# CTK_QMAKE_EXECUTABLE
# CTK_KWSTYLE_EXECUTABLE
#

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#
INCLUDE(ExternalProject)

SET(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${ep_base})

SET(ep_install_dir ${ep_base}/Install)
#SET(ep_build_dir ${ep_base}/Build)
#SET(ep_parallelism_level)
SET(ep_build_shared_libs ON)
SET(ep_build_testing OFF)

SET(ep_common_args
  -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_TESTING:BOOL=${ep_build_testing}
  )

# Compute -G arg for configuring external projects with the same CMake generator:
IF(CMAKE_EXTRA_GENERATOR)
  SET(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
ELSE()
  SET(gen "${CMAKE_GENERATOR}")
ENDIF()

# Use this value where semi-colons are needed in ep_add args:
set(sep "^^")

#-----------------------------------------------------------------------------
# Update CMake module path
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

#-----------------------------------------------------------------------------
# Qt is expected to be setup by CTK/CMakeLists.txt just before it includes the SuperBuild script
#

#-----------------------------------------------------------------------------
# KWStyle
#
SET (kwstyle_DEPENDS)
IF (CTK_USE_KWSTYLE)
  IF (NOT DEFINED CTK_KWSTYLE_EXECUTABLE)
    SET(proj KWStyle-CVSHEAD)
    SET(kwstyle_DEPENDS ${proj})
    ExternalProject_Add(${proj}
      LIST_SEPARATOR ${sep}
      CVS_REPOSITORY ":pserver:anoncvs:@public.kitware.com:/cvsroot/KWStyle"
      CVS_MODULE "KWStyle"
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
      )
    SET(CTK_KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Utilities/DCMTK
#
SET(proj DCMTK)
ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ""
    INSTALL_COMMAND "" # TODO DCMTK should be installable 
    CMAKE_GENERATOR ${gen}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/DCMTK
    CMAKE_ARGS
      ${ep_common_args}
    )

#-----------------------------------------------------------------------------
# CTK Utilities
#
set(proj CTK-Utilities)
ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    ${kwstyle_DEPENDS}
    "DCMTK"
)
  
#-----------------------------------------------------------------------------
# Convenient macro allowing to define superbuild arg
#
MACRO(ctk_set_superbuild_boolean_arg ctk_cmake_var)
  SET(superbuild_${ctk_cmake_var} ON)
  IF(DEFINED ${ctk_cmake_var} AND NOT ${ctk_cmake_var})
    SET(superbuild_${ctk_cmake_var} OFF)
  ENDIF()
  SET(superbuild_ep_arg_${ctk_cmake_var} -D${ctk_cmake_arg}:BOOL=${superbuild_${ctk_cmake_var}})
ENDMACRO()

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#

SET(ctk_cmake_boolean_args
  BUILD_TESTING
  CTK_USE_KWSTYLE
  ${ctk_libs}
  )

SET(ctk_superbuild_boolean_args)
FOREACH(ctk_cmake_arg ${ctk_cmake_boolean_args})
  ctk_set_superbuild_boolean_arg(${ctk_cmake_arg})
  LIST(APPEND ctk_superbuild_boolean_args -D${ctk_cmake_arg}:BOOL=${superbuild_${ctk_cmake_arg}})
ENDFOREACH()

#-----------------------------------------------------------------------------
# CTK Configure
#
SET(proj CTK-Configure)

ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ctk_superbuild_boolean_args}
    -DCTK_SUPERBUILD:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCTK_QMAKE_EXECUTABLE:FILEPATH=${CTK_QMAKE_EXECUTABLE}
    -DCTK_KWSTYLE_EXECUTABLE:FILEPATH=${CTK_KWSTYLE_EXECUTABLE}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}/CTK-build
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    "CTK-Utilities"
  )


#-----------------------------------------------------------------------------
# CTK
#
set(proj CTK-build)
ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  CMAKE_GENERATOR ${gen}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR CTK-build
  #BUILD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS
    "CTK-Configure"
  )
