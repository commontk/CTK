#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#

MACRO(ctkMacroGeneratePluginList )

  CtkMacroParseArguments(MY
    "FILE;DIRECTORIES"
    ""
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_FILE)
    MESSAGE(SEND_ERROR "FILE is mandatory")
  ENDIF()

  SET(_content )
  FOREACH(plugin ${MY_DIRECTORIES})
    SET(_content "${_content}\n

  ENDFOREACH()


  FILE(WRITE "${_manifest_filepath}" "${_manifest_content}")
  FILE(WRITE "${_manifest_qrc_filepath}" "${_manifest_qrc_content}")

ENDMACRO()
