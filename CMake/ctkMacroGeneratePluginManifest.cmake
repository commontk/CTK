#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

MACRO(ctkMacroGeneratePluginManifest QRC_SRCS)

  CtkMacroParseArguments(MY
    "ACTIVATIONPOLICY;CATEGORY;CONTACT_ADDRESS;COPYRIGHT;DESCRIPTION;DOC_URL;ICON;LICENSE;NAME;REQUIRE_PLUGIN;SYMBOLIC_NAME;VENDOR;VERSION"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_SYMBOLIC_NAME)
    MESSAGE(SEND_ERROR "SYMBOLIC_NAME is mandatory")
  ENDIF()

  SET(_manifest_content "Plugin-SymbolicName: ${MY_SYMBOLIC_NAME}")

  IF(DEFINED MY_ACTIVATIONPOLICY)
    STRING(TOLOWER "${MY_ACTIVATIONPOLICY}" _activation_policy)
    IF(_activation_policy STREQUAL "eager")
      SET(manifest_content "Plugin-ActivationPolicy: eager")
    ELSE()
      MESSAGE(SEND_ERROR "ACTIVATIONPOLICY is set to '${MY_ACTIVATIONPOLICY}', which is not supported")
    ENDIF()
  ENDIF()

  SET(_manifest_filename "MANIFEST.MF")
  SET(_manifest_filepath "${CMAKE_CURRENT_BINARY_DIR}/${_manifest_filename}")
  SET(_manifest_qrc_filepath "${CMAKE_CURRENT_BINARY_DIR}/${MY_SYMBOLIC_NAME}_manifest.qrc")

  SET(_manifest_qrc_content
"<!DOCTYPE RCC><RCC version=\"1.0\">
<qresource prefix=\"/${MY_SYMBOLIC_NAME}/META-INF\">
 <file>${_manifest_filename}</file>
</qresource>
</RCC>
")

  FILE(WRITE "${_manifest_filepath}" "${_manifest_content}")
  FILE(WRITE "${_manifest_qrc_filepath}" "${_manifest_qrc_content}")

  QT4_ADD_RESOURCES(${QRC_SRCS} ${_manifest_qrc_filepath})

ENDMACRO()
