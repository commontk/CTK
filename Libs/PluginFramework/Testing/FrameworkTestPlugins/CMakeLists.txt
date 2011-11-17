foreach(test_plugin ${fwtest_plugins})
  add_subdirectory(${test_plugin})
endforeach()
