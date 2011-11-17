#
# See CMake/ctkFunctionGetTargetLibraries.cmake
# 
# This file should list the libraries required to build the current CTK application.
# 

set(target_libraries
  )

## Query / Retrieve Testing
if(CTK_APP_ctkDICOMQuery AND CTK_APP_ctkDICOMRetrieve)
  list(APPEND target_libraries 
    CTKDICOMCore
    )
endif()
