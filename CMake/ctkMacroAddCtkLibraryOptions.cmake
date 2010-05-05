
#
#
#
  
MACRO(ctkMacroAddCtkLibraryOptions lib)

  SET(filepath ${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}/ctk_library_options.cmake)

  # Add options only if "ctk_library_option.cmake" file exists
  IF(EXISTS ${filepath})
  
    # Make sure the variable is cleared 
    SET(ctk_library_options )

    INCLUDE(${filepath})

    FOREACH(option ${ctk_library_options})
      ctkFunctionExtractOptionNameAndValue(${option} option_name option_value)
      OPTION(CTK_LIB_${lib}_${option_name} "Enable ${lib} Library ${option_name} option." ${option_value})
    ENDFOREACH()
    
  ENDIF()
  
ENDMACRO()
