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
  set(revision_tag "0f9bf4d9e9a778c11fdddafca691b451c2b621bc") # patched-DCMTK-3.6.6_20210115
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

  set(ep_cxx_standard_args)
  # XXX: On MSVC disable building DCMTK with C++11. DCMTK checks C++11.
  # compiler compatibility by inspecting __cplusplus, but MSVC doesn't set __cplusplus.
  # See https://blogs.msdn.microsoft.com/vcblog/2016/06/07/standards-version-switches-in-the-compiler/.
  # Microsoft: "We won’t update __cplusplus until the compiler fully conforms to
  # the standard. Until then, you can check the value of _MSVC_LANG."
  if(CMAKE_CXX_STANDARD AND UNIX)
    list(APPEND ep_cxx_standard_args
      -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
      -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
      -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
      -DDCMTK_ENABLE_CXX11:BOOL=ON
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
      ${ep_cxx_standard_args}
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
      -DDCMTK_ENABLE_BUILTIN_DICTIONARY:BOOL=ON
      -DDCMTK_ENABLE_PRIVATE_TAGS:BOOL=ON
      ${EXTERNAL_PROJECT_OPTIONAL_CMAKE_CACHE_ARGS}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  set(DCMTK_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-build)

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
