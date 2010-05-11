#
# DCMTK
#
SET(DCMTK_DEPENDS)
ctkMacroShouldAddExternalProject(DCMTK_LIBRARIES add_project)
IF(${add_project})
  IF(NOT DEFINED DCMTK_DIR)
    SET(proj DCMTK)
#     MESSAGE(STATUS "Adding project:${proj}")
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
ENDIF()