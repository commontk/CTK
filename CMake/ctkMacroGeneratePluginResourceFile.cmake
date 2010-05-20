#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

MACRO(ctkMacroGeneratePluginResourceFile QRC_SRCS)

  CtkMacroParseArguments(MY
    "NAME;PREFIX;RESOURCES"
    ""
    ${ARGN}
    )

  SET(_qrc_filepath "${CMAKE_CURRENT_BINARY_DIR}/${MY_NAME}")

  SET(_qrc_content
"<!DOCTYPE RCC><RCC version=\"1.0\">
<qresource prefix=\"/${MY_PREFIX}\">
")

  FOREACH(_resource_file ${MY_RESOURCES})
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/${_resource_file}" "${CMAKE_CURRENT_BINARY_DIR}/${_resource_file}" COPYONLY)
    SET(_qrc_content "${_qrc_content}<file>${_resource_file}</file>
")
  ENDFOREACH()

  SET(_qrc_content "${_qrc_content}</qresource>
</RCC>
")
  FILE(WRITE "${_qrc_filepath}" "${_qrc_content}")

  QT4_ADD_RESOURCES(${QRC_SRCS} ${_qrc_filepath})

ENDMACRO()
