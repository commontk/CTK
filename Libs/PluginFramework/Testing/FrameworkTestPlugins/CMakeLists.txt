foreach(test_plugin ${fwtest_plugins})
  ADD_SUBDIRECTORY(${test_plugin})
endforeach()
