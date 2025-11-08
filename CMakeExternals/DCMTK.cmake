#
# DCMTK
#

set(proj DCMTK)

set(${proj}_DEPENDENCIES "")

ExternalProject_Include_Dependencies(${proj}
  PROJECT_VAR proj
  DEPENDS_VAR ${proj}_DEPENDENCIES
  EP_ARGS_VAR ${proj}_EXTERNAL_PROJECT_ARGS
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

# Let the user to chose whether to use libs installed in the system
option(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} "Use installed ${proj} library in the system" OFF)
mark_as_advanced(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(DCMTK_DIR CACHE)
  find_package(DCMTK REQUIRED)
endif()

# Sanity checks
if(DEFINED DCMTK_DIR AND NOT EXISTS ${DCMTK_DIR})
  message(FATAL_ERROR "DCMTK_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED DCMTK_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  set(revision_tag "f390821c469c0897c1b5437164c8826e080a2581") # patched-DCMTK-3.6.8_20241024
  if(${proj}_REVISION_TAG)
    set(revision_tag ${${proj}_REVISION_TAG})
  endif()

  set(location_args )
  if(${proj}_URL)
    set(location_args URL ${${proj}_URL})
  elseif(${proj}_GIT_REPOSITORY)
    set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                      GIT_TAG ${revision_tag})
  else()
    set(location_args GIT_REPOSITORY "${EP_GIT_PROTOCOL}://github.com/commontk/DCMTK.git"
                      GIT_TAG ${revision_tag})
  endif()

  set(EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS)

  if(UNIX)
    list(APPEND EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS
      -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON
      -DDCMTK_WITH_WRAP:BOOL=OFF   # CTK does not build on Mac with this option turned ON due to library dependencies missing
      )
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EXTERNAL_PROJECT_ARGS}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    ${location_args}
    INSTALL_COMMAND ""
    CMAKE_ARGS
      -DDCMTK_INSTALL_BINDIR:STRING=bin/${CMAKE_CFG_INTDIR}
      -DDCMTK_INSTALL_LIBDIR:STRING=lib/${CMAKE_CFG_INTDIR}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DDCMTK_WITH_DOXYGEN:BOOL=OFF
      -DDCMTK_WITH_ZLIB:BOOL=OFF # see github issue #25
      -DDCMTK_WITH_OPENSSL:BOOL=OFF # see github issue #25
      -DDCMTK_WITH_PNG:BOOL=OFF # see github issue #25
      -DDCMTK_WITH_TIFF:BOOL=OFF  # see github issue #25
      -DDCMTK_WITH_XML:BOOL=OFF  # see github issue #25
      -DDCMTK_WITH_ICONV:BOOL=OFF  # see github issue #178
      -DDCMTK_WITH_SNDFILE:BOOL=OFF # see DCMQI github issue #395
      -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF
      -DDCMTK_DEFAULT_DICT:STRING=builtin
      -DDCMTK_ENABLE_PRIVATE_TAGS:BOOL=ON

      ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(DCMTK_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-build)

  #-----------------------------------------------------------------------------
  # Launcher setting specific to build tree

  set(_lib_subdir lib)
  if(WIN32)
    set(_lib_subdir bin)
  endif()

  # library paths
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD ${DCMTK_DIR}/${_lib_subdir}/<CMAKE_CFG_INTDIR>)
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
    )

  # paths
  set(${proj}_PATHS_LAUNCHER_BUILD ${DCMTK_DIR}/bin/<CMAKE_CFG_INTDIR>)
  mark_as_superbuild(
    VARS ${proj}_PATHS_LAUNCHER_BUILD
    LABELS "PATHS_LAUNCHER_BUILD"
    )

  #-----------------------------------------------------------------------------
  # Launcher setting specific to install tree

  # NA

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS DCMTK_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

# If an external DCMTK was provided via DCMTK_DIR and the external DCMTK
# build/install used a CMAKE_DEBUG_POSTFIX value for distinguishing debug
# and release libraries in the same build/install tree, the same debug
# postfix needs to be passed to the CTK configure step. The FindDCMTK
# script then takes the DCMTK_CMAKE_DEBUG_POSTFIX variable into account
# when looking for DCMTK debug libraries.
mark_as_superbuild(DCMTK_CMAKE_DEBUG_POSTFIX:STRING)
