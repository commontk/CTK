SET(fw_lib ${PROJECT_NAME} CACHE INTERNAL "Name of the PluginFramework library")
SET(fwtestutil_lib ${PROJECT_NAME}TestUtil CACHE INTERNAL "Name of the PluginFramework test utility library")

ADD_SUBDIRECTORY(Cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Cpp)

SET(fwtest_plugins
  pluginA_test
  pluginS_test
  pluginA2_test
  pluginD_test
  pluginSL1_test
  pluginSL3_test
  pluginSL4_test
)

SET(metatypetest_plugins
  pluginAttrPwd_test
)

ADD_SUBDIRECTORY(FrameworkTest)
ADD_SUBDIRECTORY(FrameworkTestPlugins)

ADD_SUBDIRECTORY(ConfigAdminTest)
ADD_SUBDIRECTORY(EventAdminTest)

ADD_SUBDIRECTORY(MetaTypeTest)
ADD_SUBDIRECTORY(MetaTypeTestPlugins)
