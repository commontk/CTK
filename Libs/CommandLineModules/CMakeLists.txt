
find_package(Doxygen)

if(DOXYGEN_FOUND)
  add_subdirectory(Documentation)
endif()

if(BUILD_TESTING AND
    CTK_LIB_CommandLineModules/Core AND CTK_LIB_CommandLineModules/Backend/LocalProcess)
  add_subdirectory(Testing)
endif()
