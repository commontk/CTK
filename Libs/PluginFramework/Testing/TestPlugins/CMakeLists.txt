set(test_plugins
  pluginA_test
  pluginA2_test
  pluginSL1_test
  pluginSL3_test
  pluginSL4_test
)

foreach(test_plugin ${test_plugins})
  ADD_SUBDIRECTORY(${test_plugin})
endforeach()
