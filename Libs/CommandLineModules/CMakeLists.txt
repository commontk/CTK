
if(BUILD_TESTING AND
    CTK_LIB_CommandLineModules/Core AND CTK_LIB_CommandLineModules/Backend/LocalProcess)
  add_subdirectory(Testing)
endif()

add_subdirectory(Documentation)
