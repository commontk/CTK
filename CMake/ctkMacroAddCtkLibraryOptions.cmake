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
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#! \ingroup CMakeUtilities
macro(ctkMacroAddCtkLibraryOptions lib)

  set(filepath ${CMAKE_CURRENT_SOURCE_DIR}/Libs/${lib}/ctk_library_options.cmake)

  # Add options only if "ctk_library_option.cmake" file exists
  if(EXISTS ${filepath})
  
    # Make sure the variable is cleared 
    set(ctk_library_options )

    include(${filepath})

    foreach(option ${ctk_library_options})
      ctkFunctionExtractOptionNameAndValue(${option} option_name option_value)
      option(CTK_LIB_${lib}_${option_name} "Enable ${lib} Library ${option_name} option." ${option_value})
      mark_as_advanced(CTK_LIB_${lib}_${option_name})
      mark_as_superbuild(CTK_LIB_${lib}_${option_name})
    endforeach()
    
  endif()
  
endmacro()
