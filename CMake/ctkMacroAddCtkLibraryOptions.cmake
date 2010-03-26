
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

      # Make sure option is correctly formated
      IF(NOT "${option}" MATCHES "^[A-Za-z0-9]+:(ON|OFF)")
        MESSAGE(FATAL_ERROR "In ${filepath}, option ${option} is incorrect. Options should be specified using the following format OPT1:[ON|OFF]. For example OPT1:OFF or OPT2:ON")
      ENDIF()
      
      # Extract option name and option default value
      STRING(REPLACE ":" "\\;" option ${option})
      SET(optionlist ${option})
      LIST(GET optionlist 0 option_name)
      LIST(GET optionlist 1 option_value)

      OPTION(CTK_LIB_${lib}_${option_name} "Enable ${lib} Library ${option_name} option." ${option_value})
    ENDFOREACH()
    
  ENDIF()
  
ENDMACRO()
