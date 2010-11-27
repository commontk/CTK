

SET(KIT_PYTHON_SCRIPTS
  qt/__init__
  )
  
#IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
#  LIST(APPEND KIT_PYTHON_SCRIPTS ctkvtk/__init__)
#ENDIF()

IF(CTK_WRAP_PYTHONQT_LIGHT OR CTK_WRAP_PYTHONQT_FULL)

  # Configure variable CTK_PYTHON_WRAPPED_LIBRARIES that will contain
  # the comma separated list of package to load
  SET(CTK_PYTHON_WRAPPED_LIBRARIES)
  FOREACH(lib ${CTK_LIBS})
    ctkFunctionExtractOptionNameAndValue(${lib} lib_name lib_value)
    IF(${CTK_LIB_${lib_name}})
      STRING(REPLACE "/" "" lib_name_no_slash ${lib_name})
      SET(lib_name_no_slash "'${lib_name_no_slash}'") # Add single quotes
      SET(CTK_PYTHON_WRAPPED_LIBRARIES "${lib_name_no_slash}, ${CTK_PYTHON_WRAPPED_LIBRARIES}")
    ENDIF()
  ENDFOREACH()

  CONFIGURE_FILE(
    ctk/__init__.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/ctk/__init__.py
    @ONLY
    )

  LIST(APPEND KIT_PYTHON_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/ctk/__init__.py)
ENDIF()

SET(KIT_PYTHON_RESOURCES
  )

ctkMacroCompilePythonScript(
  TARGET_NAME ${PROJECT_NAME}
  SCRIPTS "${KIT_PYTHON_SCRIPTS}"
  RESOURCES "${KIT_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CTK_BINARY_DIR}/bin/Python
  INSTALL_DIR ${CTK_INSTALL_BIN_DIR}
  )
  
