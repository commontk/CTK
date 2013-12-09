#
# LibD
#

superbuild_include_once()

set(LibD_DEPENDENCIES "")

superbuild_include_dependencies(LibD)
set(proj LibD)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})
  unset(LibD_DIR CACHE)
endif()

mark_as_superbuild(
  VARS ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}:BOOL
  LABELS "USE_SYSTEM"
  )

# Sanity checks
if(DEFINED LibD_DIR AND NOT EXISTS ${LibD_DIR})
  message(FATAL_ERROR "LibD_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED LibD_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj})

  message(FATAL_ERROR "Disabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj} is not supported !")

else()
  superbuild_add_empty_external_project(${proj} "${${proj}_DEPENDENCIES}")
endif()

