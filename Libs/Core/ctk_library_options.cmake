#
# See CMake/ctkMacroAddCtkLibraryOptions.cmake
# 
# This file should list of options available for considered CTK library
# For example: MYOPT1:OFF MYOPT2:ON
# 

# Note: Options WITH_BFD_SHARED and WITH_BFD_STATIC are mutually exclusive. 
#       Enabling both options will trigger a configuration error.

set(ctk_library_options
  WITH_BFD_SHARED:OFF
  WITH_BFD_STATIC:OFF
  )
  
