if(BUILD_TESTING)
  include_directories(${CTKPluginFramework_SOURCE_DIR})
  ctkFunctionCompileSnippets("${CMAKE_CURRENT_SOURCE_DIR}" CTKPluginFramework)
endif()
