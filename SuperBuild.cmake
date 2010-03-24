SET(cmake_version_required "2.8")
SET(cmake_version_required_dash "2-8")

CMAKE_MINIMUM_REQUIRED(VERSION ${cmake_version_required})

# 
# CTK_KWSTYLE_EXECUTABLE
# DCMTK_DIR
# QT_QMAKE_EXECUTABLE
#

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#
INCLUDE(ExternalProject)

SET(ep_base "${CMAKE_BINARY_DIR}/CMakeExternals")
SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${ep_base})

SET(ep_install_dir ${ep_base}/Install)
SET(ep_build_dir ${ep_base}/Build)
SET(ep_source_dir ${ep_base}/Source)
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
# PythonQt
#
SET (PythonQt_DEPENDS)
# IF ()
#   SET(proj PythonQt)
#   SET(PythonQt_DEPENDS ${proj})
#   ExternalProject_Add(${proj}
#       SVN_REPOSITORY "https://pythonqt.svn.sourceforge.net/svnroot/pythonqt/trunk"
#       CMAKE_GENERATOR ${gen}
#       PATCH_COMMAND ${CMAKE_COMMAND} -P ${pythonqt_patch_script}
#       CMAKE_ARGS
#         ${ep_common_args}
#         -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
#         #${vtk_PYTHON_ARGS}
#       INSTALL_COMMAND "")
# ENDIF()
    
#-----------------------------------------------------------------------------
# Utilities/DCMTK
#
SET (DCMTK_DEPENDS)
IF (NOT DEFINED DCMTK_DIR)
  SET(proj DCMTK)
  SET(DCMTK_DEPENDS ${proj})
  ExternalProject_Add(${proj}
      DOWNLOAD_COMMAND ""
      CMAKE_GENERATOR ${gen}
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/${proj}
      CMAKE_ARGS
        ${ep_common_args}
        -DDCMTK_BUILD_APPS:BOOL=ON # Build also dmctk tools (movescu, storescp, ...)
      )
  SET(DCMTK_DIR ${ep_install_dir})
ENDIF()

#-----------------------------------------------------------------------------
# Utilities/ZMQ
#
SET(proj ZMQ)
ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${gen}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/ZMQ
    CMAKE_ARGS
      ${ep_common_args}
    )

#-----------------------------------------------------------------------------
# QtMobility
#
SET(proj QtMobility)

# Configure patch script
SET(qtmobility_src_dir ${ep_source_dir}/${proj})
SET(qtmobility_patch_dir ${CTK_SOURCE_DIR}/Utilities/QtMobility/)
SET(qtmobility_configured_patch_dir ${CTK_BINARY_DIR}/Utilities/QtMobility/)
SET(qtmobility_patchscript
  ${CTK_BINARY_DIR}/Utilities/QtMobility/QtMobilityBeta1-patch.cmake)
CONFIGURE_FILE(
  ${CTK_SOURCE_DIR}/Utilities/QtMobility/QtMobilityBeta1-patch.cmake.in
  ${qtmobility_patchscript} @ONLY)

# Define configure options
SET(qtmobility_modules "serviceframework")
SET(qtmobility_build_type "release")
IF(UNIX)
 IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET(qtmobility_build_type "debug")
 ENDIF()
ELSEIF(NOT ${CMAKE_CFG_INTDIR} STREQUAL "Release")
 SET(qtmobility_build_type "debug")
ENDIf()

ExternalProject_Add(${proj}
   URL "http://get.qt.nokia.com/qt/solutions/qt-mobility-src-1.0.0-beta1.tar.gz"
   CONFIGURE_COMMAND <SOURCE_DIR>/configure -${qtmobility_build_type} -libdir ${CMAKE_BINARY_DIR}/CTK-build/bin -no-docs -modules ${qtmobility_modules}
   PATCH_COMMAND ${CMAKE_COMMAND} -P ${qtmobility_patchscript}
   BUILD_IN_SOURCE 1
   )

#-----------------------------------------------------------------------------
# Utilities/OpenIGTLink
#
SET(proj OpenIGTLink)
ExternalProject_Add(${proj}
    SVN_REPOSITORY "http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink"
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${ep_common_args}
    )

#-----------------------------------------------------------------------------
# XIP
#
# SET(proj XIP)
# SET(url https://collab01a.scr.siemens.com/svn/xip/releases/latest)
# ExternalProject_Add(${proj}
#    DOWNLOAD_COMMAND "${CMAKE_COMMAND} -E ${SVNCOMMAND} checkout ${url} ${ep_source_dir}/${proj} --username=anonymous "
#    UPDATE_COMMAND ""
#    #SVN_REPOSITORY "https://anonymous@collab01a.scr.siemens.com/svn/xip/releases/latest"
#    INSTALL_COMMAND ""
#    CMAKE_GENERATOR ${gen}
#    CMAKE_ARGS
#      ${ep_common_args}
#    )
   
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
    ${DCMTK_DEPENDS}
    ${PythonQt_DEPENDS}
    ZMQ
    OpenIGTLink
#     XIP
    QtMobility
)

#-----------------------------------------------------------------------------
# Generate cmake variable name corresponding to Libs, Plugins and Applications
#
SET(ctk_libs_bool_vars)
FOREACH(lib ${ctk_libs})
  LIST(APPEND ctk_libs_bool_vars CTK_LIB_${lib})
ENDFOREACH()

SET(ctk_plugins_bool_vars)
FOREACH(plugin ${ctk_plugins})
  LIST(APPEND ctk_plugins_bool_vars CTK_PLUGIN_${plugin})
ENDFOREACH()

SET(ctk_applications_bool_vars)
FOREACH(app ${ctk_applications})
  LIST(APPEND ctk_applications_bool_vars CTK_APP_${app})
ENDFOREACH()

#-----------------------------------------------------------------------------
# Convenient macro allowing to define superbuild arg
#
MACRO(ctk_set_superbuild_boolean_arg ctk_cmake_var)
  SET(superbuild_${ctk_cmake_var} ON)
  IF(DEFINED ${ctk_cmake_var} AND NOT ${ctk_cmake_var})
    SET(superbuild_${ctk_cmake_var} OFF)
  ENDIF()
ENDMACRO()

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#

SET(ctk_cmake_boolean_args
  BUILD_TESTING
  CTK_USE_KWSTYLE
  ${ctk_libs_bool_vars}
  ${ctk_plugins_bool_vars}
  ${ctk_applications_bool_vars}
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
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    -DCTK_KWSTYLE_EXECUTABLE:FILEPATH=${CTK_KWSTYLE_EXECUTABLE}
    -DDCMTK_DIR=${DCMTK_DIR} # FindDCMTK expects DCMTK_DIR
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
