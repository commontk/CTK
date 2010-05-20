#
# log4cpp
#
SET(log4cpp_DEPENDS)
ctkMacroShouldAddExternalProject(log4cpp_LIBRARIES add_project)
IF(${add_project})
  IF(NOT DEFINED log4cpp_DIR)
    SET(proj log4cpp)
#     MESSAGE(STATUS "Adding project:${proj}")
    SET(log4cpp_DEPENDS ${proj})
    ExternalProject_Add(${proj}
        DOWNLOAD_COMMAND ""
        CMAKE_GENERATOR ${gen}
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/${proj}
        CMAKE_ARGS
          ${ep_common_args}
        )
    SET(log4cpp_DIR ${ep_install_dir})
  ENDIF()
ENDIF()
