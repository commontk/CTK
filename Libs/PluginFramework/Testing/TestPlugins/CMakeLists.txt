set(test_plugins
  pluginA_test
)

foreach(test_plugin ${test_plugins})
  ADD_SUBDIRECTORY(${test_plugin})
endforeach()
