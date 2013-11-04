

set(KIT_PYTHON_SCRIPTS
  qt/__init__
  )

# If it applies, the variable CTK_PYTHON_WRAPPED_LIBRARIES will contain
# the comma separated list of package to load
set(CTK_PYTHON_WRAPPED_LIBRARIES)

if(CTK_WRAP_PYTHONQT_LIGHT OR CTK_WRAP_PYTHONQT_FULL)

  foreach(lib_name ${CTK_LIBS})
    if(${CTK_LIB_${lib_name}})
      string(REPLACE "/" "" lib_name_no_slash ${lib_name})
      set(lib_name_no_slash "'${lib_name_no_slash}'") # Add single quotes
      set(CTK_PYTHON_WRAPPED_LIBRARIES "${lib_name_no_slash}, ${CTK_PYTHON_WRAPPED_LIBRARIES}")
    endif()
  endforeach()

endif()

configure_file(
  ctk/__init__.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctk/__init__.py
  @ONLY
  )
list(APPEND KIT_PYTHON_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/ctk/__init__.py)

set(KIT_PYTHON_RESOURCES
  )

ctkMacroCompilePythonScript(
  TARGET_NAME ${PROJECT_NAME}
  SCRIPTS "${KIT_PYTHON_SCRIPTS}"
  RESOURCES "${KIT_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CTK_BINARY_DIR}/bin/Python
  INSTALL_DIR ${CTK_INSTALL_BIN_DIR}
  )

