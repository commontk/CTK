set(fw_lib ${PROJECT_NAME} CACHE INTERNAL "Name of the PluginFramework library")
set(fwtestutil_lib ${PROJECT_NAME}TestUtil CACHE INTERNAL "Name of the PluginFramework test utility library")

add_subdirectory(Cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Cpp)

set(fwtest_plugins
  pluginA_test
  pluginS_test
  pluginA2_test
  pluginD_test
  pluginSL1_test
  pluginSL3_test
  pluginSL4_test
)

set(metatypetest_plugins
  pluginAttrPwd_test
)

add_subdirectory(FrameworkTest)
add_subdirectory(FrameworkTestPlugins)

add_subdirectory(ConfigAdminTest)
add_subdirectory(EventAdminTest)

add_subdirectory(MetaTypeTest)
add_subdirectory(MetaTypeTestPlugins)
