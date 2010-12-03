ADD_SUBDIRECTORY(Cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Cpp)

SET(test_plugins
  pluginA_test
  pluginA2_test
  pluginD_test
  pluginSL1_test
  pluginSL3_test
  pluginSL4_test
)

ADD_SUBDIRECTORY(FrameworkTest)
ADD_SUBDIRECTORY(TestPlugins)
