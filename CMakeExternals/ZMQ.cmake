#
# ZMQ
#
SET(ZMQ_DEPENDS)
ctkMacroShouldAddExternalProject(ZMQ_LIBRARIES add_project)
IF(${add_project})
  SET(proj ZMQ)
#   MESSAGE(STATUS "Adding project:${proj}")
  SET(ZMQ_DEPENDS ${proj})
  ExternalProject_Add(${proj}
      DOWNLOAD_COMMAND ""
      INSTALL_COMMAND ""
      CMAKE_GENERATOR ${gen}
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/ZMQ
      CMAKE_ARGS
        ${ep_common_args}
      )
ENDIF()