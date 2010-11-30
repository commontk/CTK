#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

FUNCTION(ctkFunctionGeneratePluginManifest QRC_SRCS)

  CtkMacroParseArguments(MY
    "ACTIVATIONPOLICY;CATEGORY;CONTACT_ADDRESS;COPYRIGHT;DESCRIPTION;DOC_URL;ICON;LICENSE;NAME;REQUIRE_PLUGIN;SYMBOLIC_NAME;VENDOR;VERSION;CUSTOM_HEADERS"
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
      SET(_manifest_content "${_manifest_content}\nPlugin-ActivationPolicy: eager")
    ELSE()
      MESSAGE(SEND_ERROR "ACTIVATIONPOLICY is set to '${MY_ACTIVATIONPOLICY}', which is not supported")
    ENDIF()
  ENDIF()

  IF(DEFINED MY_CATEGORY)
    SET(_manifest_content "${_manifest_content}\nPlugin-Category: ${MY_CATEGORY}")
  ENDIF()

  IF(DEFINED MY_CONTACT_ADDRESS)
    SET(_manifest_content "${_manifest_content}\nPlugin-ContactAddress: ${MY_CONTACT_ADDRESS}")
  ENDIF()

  IF(DEFINED MY_COPYRIGHT)
    SET(_manifest_content "${_manifest_content}\nPlugin-Copyright: ${MY_COPYRIGHT}")
  ENDIF()

  IF(DEFINED MY_DESCRIPTION)
    SET(_manifest_content "${_manifest_content}\nPlugin-Description: ${MY_DESCRIPTION}")
  ENDIF()

  IF(DEFINED MY_DOC_URL)
    SET(_manifest_content "${_manifest_content}\nPlugin-DocURL: ${MY_DOC_URL}")
  ENDIF()

  IF(DEFINED MY_ICON)
    SET(_manifest_content "${_manifest_content}\nPlugin-Icon: ${MY_ICON}")
  ENDIF()

  IF(DEFINED MY_LICENSE)
    SET(_manifest_content "${_manifest_content}\nPlugin-License: ${MY_LICENSE}")
  ENDIF()

  IF(DEFINED MY_NAME)
    SET(_manifest_content "${_manifest_content}\nPlugin-Name: ${MY_NAME}")
  ENDIF()

  IF(DEFINED MY_REQUIRE_PLUGIN)
    STRING(REPLACE ";" "," require_plugin "${MY_REQUIRE_PLUGIN}")
    SET(_manifest_content "${_manifest_content}\nRequire-Plugin: ${require_plugin}")
  ENDIF()

  IF(DEFINED MY_VENDOR)
    SET(_manifest_content "${_manifest_content}\nPlugin-Vendor: ${MY_VENDOR}")
  ENDIF()

  IF(DEFINED MY_VERSION)
    SET(_manifest_content "${_manifest_content}\nPlugin-Version: ${MY_VERSION}")
  ENDIF()

  IF(DEFINED MY_CUSTOM_HEADERS)
    SET(_manifest_content "${_manifest_content}\n")
    FOREACH(_custom_header ${MY_CUSTOM_HEADERS})
      SET(_manifest_content "${_manifest_content}\n${_custom_header}: ${${_custom_header}}")
    ENDFOREACH()
  ENDIF()

  SET(_manifest_filename "MANIFEST.MF")
  SET(_manifest_filepath "${CMAKE_CURRENT_BINARY_DIR}/${_manifest_filename}")
  STRING(REPLACE "." "_" _symbolic_name ${MY_SYMBOLIC_NAME})
  SET(_manifest_qrc_filepath "${CMAKE_CURRENT_BINARY_DIR}/${_symbolic_name}_manifest.qrc")

  SET(_manifest_qrc_content
"<!DOCTYPE RCC><RCC version=\"1.0\">
<qresource prefix=\"/${MY_SYMBOLIC_NAME}/META-INF\">
 <file>${_manifest_filename}</file>
</qresource>
</RCC>
")

  FILE(WRITE "${_manifest_filepath}" "${_manifest_content}")
  FILE(WRITE "${_manifest_qrc_filepath}" "${_manifest_qrc_content}")

  QT4_ADD_RESOURCES(_qrc_src ${_manifest_qrc_filepath})

  SET(${QRC_SRCS} ${${QRC_SRCS}} ${_qrc_src} PARENT_SCOPE)

ENDFUNCTION()
