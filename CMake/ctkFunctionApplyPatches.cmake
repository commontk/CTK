#
#
#

FUNCTION(ctkFunctionApplyPatches PATCH_EXE SRC_DIR PATCH_FILES)
  
  # Make sure patch executable exists
  IF(NOT EXISTS ${PATCH_EXE})
    MESSAGE(FATAL_ERROR "ERROR: Program [${PATCH_EXE}] doesn't exist !")
  ENDIF()

  # Apply patches
  FOREACH(patch_file ${PATCH_FILES})
    MESSAGE("Applying patch: ${patch_file}")
    EXECUTE_PROCESS(COMMAND ${PATCH_EXE} -p0 -i "${patch_file}" -d "${SRC_DIR}" RESULT_VARIABLE result_var)
    IF(result_var)
      MESSAGE("ERROR: ${result_var}")
    ENDIF(result_var)
  ENDFOREACH()
  
ENDFUNCTION()
