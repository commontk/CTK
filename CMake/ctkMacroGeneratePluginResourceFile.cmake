#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

#! \ingroup CMakeUtilities
macro(ctkMacroGeneratePluginResourceFile QRC_SRCS)

  CtkMacroParseArguments(MY
    "NAME;PREFIX;RESOURCES;BINARY_RESOURCES"
    ""
    ${ARGN}
    )

  set(_qrc_filepath "${CMAKE_CURRENT_BINARY_DIR}/${MY_NAME}")

  set(_qrc_content
"<!DOCTYPE RCC><RCC version=\"1.0\">
<qresource prefix=\"/${MY_PREFIX}\">
")

  if(MY_RESOURCES)
    foreach(_resource_file ${MY_RESOURCES})
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_resource_file}" "${CMAKE_CURRENT_BINARY_DIR}/${_resource_file}" COPYONLY)
      set(_qrc_content "${_qrc_content}<file>${_resource_file}</file>
")
    endforeach()
  endif()

  if(MY_BINARY_RESOURCES)
    foreach(_resource_file ${MY_BINARY_RESOURCES})
      set(_qrc_content "${_qrc_content}<file>${_resource_file}</file>
")
    endforeach()
  endif()

  set(_qrc_content "${_qrc_content}</qresource>
</RCC>
")
  configure_file("${CTK_CMAKE_DIR}/plugin_resources_cached.qrc.in" "${_qrc_filepath}" @ONLY)

  if (CTK_QT_VERSION VERSION_GREATER "4")
    QT5_ADD_RESOURCES(${QRC_SRCS} ${_qrc_filepath})
  else()
    QT4_ADD_RESOURCES(${QRC_SRCS} ${_qrc_filepath})
  endif()

endmacro()
