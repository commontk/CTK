set(fw_lib ${PROJECT_NAME} CACHE INTERNAL "Name of the PluginFramework library")
set(fwtestutil_lib ${PROJECT_NAME}TestUtil CACHE INTERNAL "Name of the PluginFramework test utility library")

add_subdirectory(Cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Cpp)

set(fwtest_plugins
  pluginA_test
  pluginA1_test
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

add_subdirectory(org.commontk.pluginfwtest)
add_subdirectory(FrameworkTestPlugins)

add_subdirectory(org.commontk.pluginfwtest.perf)
add_subdirectory(org.commontk.eventadmintest.perf)

add_subdirectory(org.commontk.configadmintest)
add_subdirectory(org.commontk.eventadmintest)

add_subdirectory(org.commontk.metatypetest)
add_subdirectory(MetaTypeTestPlugins)
