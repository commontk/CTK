###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.commontk.org/LICENSE
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

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
      MARK_AS_ADVANCED(CTK_LIB_${lib}_${option_name})
      LIST(APPEND ctk_lib_options_list CTK_LIB_${lib}_${option_name})
    ENDFOREACH()
    
  ENDIF()
  
ENDMACRO()
