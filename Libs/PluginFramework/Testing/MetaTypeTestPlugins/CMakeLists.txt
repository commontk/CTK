foreach(test_plugin ${metatypetest_plugins})
  ADD_SUBDIRECTORY(${test_plugin})
endforeach()
