if(BUILD_TESTING)
  include_directories(${CTKCore_SOURCE_DIR})
  ctkFunctionCompileSnippets("${CMAKE_CURRENT_SOURCE_DIR}" CTKCore)
endif()
