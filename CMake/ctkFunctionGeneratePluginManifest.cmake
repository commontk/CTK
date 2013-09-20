#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

#! \ingroup CMakeUtilities
function(ctkFunctionGeneratePluginManifest QRC_SRCS)

  CtkMacroParseArguments(MY
    "ACTIVATIONPOLICY;CATEGORY;CONTACT_ADDRESS;COPYRIGHT;DESCRIPTION;DOC_URL;ICON;LICENSE;NAME;REQUIRE_PLUGIN;SYMBOLIC_NAME;VENDOR;VERSION;CUSTOM_HEADERS"
    ""
    ${ARGN}
    )

  # Sanity checks
  if(NOT DEFINED MY_SYMBOLIC_NAME)
    message(FATAL_ERROR "SYMBOLIC_NAME is mandatory")
  endif()

  set(_manifest_content "Plugin-SymbolicName: ${MY_SYMBOLIC_NAME}")

  if(DEFINED MY_ACTIVATIONPOLICY)
    string(TOLOWER "${MY_ACTIVATIONPOLICY}" _activation_policy)
    if(_activation_policy STREQUAL "eager")
      set(_manifest_content "${_manifest_content}\nPlugin-ActivationPolicy: eager")
    else()
      message(FATAL_ERROR "ACTIVATIONPOLICY is set to '${MY_ACTIVATIONPOLICY}', which is not supported")
    endif()
  endif()

  if(DEFINED MY_CATEGORY)
    set(_manifest_content "${_manifest_content}\nPlugin-Category: ${MY_CATEGORY}")
  endif()

  if(DEFINED MY_CONTACT_ADDRESS)
    set(_manifest_content "${_manifest_content}\nPlugin-ContactAddress: ${MY_CONTACT_ADDRESS}")
  endif()

  if(DEFINED MY_COPYRIGHT)
    set(_manifest_content "${_manifest_content}\nPlugin-Copyright: ${MY_COPYRIGHT}")
  endif()

  if(DEFINED MY_DESCRIPTION)
    set(_manifest_content "${_manifest_content}\nPlugin-Description: ${MY_DESCRIPTION}")
  endif()

  if(DEFINED MY_DOC_URL)
    set(_manifest_content "${_manifest_content}\nPlugin-DocURL: ${MY_DOC_URL}")
  endif()

  if(DEFINED MY_ICON)
    set(_manifest_content "${_manifest_content}\nPlugin-Icon: ${MY_ICON}")
  endif()

  if(DEFINED MY_LICENSE)
    set(_manifest_content "${_manifest_content}\nPlugin-License: ${MY_LICENSE}")
  endif()

  if(DEFINED MY_NAME)
    set(_manifest_content "${_manifest_content}\nPlugin-Name: ${MY_NAME}")
  endif()

  if(DEFINED MY_REQUIRE_PLUGIN)
    string(REPLACE ";" "," require_plugin "${MY_REQUIRE_PLUGIN}")
    set(_manifest_content "${_manifest_content}\nRequire-Plugin: ${require_plugin}")
  endif()

  if(DEFINED MY_VENDOR)
    set(_manifest_content "${_manifest_content}\nPlugin-Vendor: ${MY_VENDOR}")
  endif()

  if(DEFINED MY_VERSION)
    set(_manifest_content "${_manifest_content}\nPlugin-Version: ${MY_VERSION}")
  endif()

  if(DEFINED MY_CUSTOM_HEADERS)
    set(_manifest_content "${_manifest_content}\n")
    foreach(_custom_header ${MY_CUSTOM_HEADERS})
      set(_manifest_content "${_manifest_content}\n${_custom_header}: ${${_custom_header}}")
    endforeach()
  endif()

  set(_manifest_filename "MANIFEST.MF")
  set(_manifest_filepath "${CMAKE_CURRENT_BINARY_DIR}/${_manifest_filename}")
  string(REPLACE "." "_" _symbolic_name ${MY_SYMBOLIC_NAME})
  set(_manifest_qrc_filepath "${CMAKE_CURRENT_BINARY_DIR}/${_symbolic_name}_manifest.qrc")

  set(_manifest_qrc_content
"<!DOCTYPE RCC><RCC version=\"1.0\">
<qresource prefix=\"/${MY_SYMBOLIC_NAME}/META-INF\">
 <file>${_manifest_filename}</file>
</qresource>
</RCC>
")

  configure_file("${CTK_CMAKE_DIR}/MANIFEST.MF.in" "${_manifest_filepath}" @ONLY)
  configure_file("${CTK_CMAKE_DIR}/plugin_manifest.qrc.in" "${_manifest_qrc_filepath}" @ONLY)

  if (CTK_QT_VERSION VERSION_GREATER "4")
    QT5_ADD_RESOURCES(_qrc_src ${_manifest_qrc_filepath})
  else()
    QT4_ADD_RESOURCES(_qrc_src ${_manifest_qrc_filepath})
  endif()

  set(${QRC_SRCS} ${${QRC_SRCS}} ${_qrc_src} PARENT_SCOPE)

endfunction()
