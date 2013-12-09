#
# LibC
#

superbuild_include_once()

set(proj LibC)

set(${proj}_DEPENDENCIES LibD)

superbuild_include_dependencies(PROJECT_VAR proj)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(LibC_DIR CACHE)
endif()

mark_as_superbuild(
  VARS ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}:BOOL
  LABELS "USE_SYSTEM"
  )

# Sanity checks
if(DEFINED LibC_DIR AND NOT EXISTS ${LibC_DIR})
  message(FATAL_ERROR "LibC_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED LibC_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

   message(FATAL_ERROR "Disabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")

else()
  superbuild_add_empty_external_project(${proj} "${${proj}_DEPENDENCIES}")
endif()

