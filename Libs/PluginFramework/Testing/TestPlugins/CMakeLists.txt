foreach(test_plugin ${test_plugins})
  ADD_SUBDIRECTORY(${test_plugin})
endforeach()
