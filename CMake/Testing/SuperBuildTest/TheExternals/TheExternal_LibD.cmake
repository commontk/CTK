#
# LibD
#

superbuild_include_once()

set(proj_libd LibD)

set(${proj_libd}_DEPENDENCIES "")

superbuild_include_dependencies(PROJECT_VAR proj_libd)

if(${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj_libd})
  unset(LibD_DIR CACHE)
endif()

mark_as_superbuild(
  VARS ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj_libd}:BOOL
  LABELS "USE_SYSTEM"
  )

# Sanity checks
if(DEFINED LibD_DIR AND NOT EXISTS ${LibD_DIR})
  message(FATAL_ERROR "LibD_DIR variable is defined but corresponds to non-existing directory")
endif()

if(NOT DEFINED LibD_DIR AND NOT ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj_libd})

  message(FATAL_ERROR "Disabling ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj_libd} is not supported !")

else()
  superbuild_add_empty_external_project(${proj_libd} "${${proj_libd}_DEPENDENCIES}")
endif()

