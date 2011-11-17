foreach(test_plugin ${metatypetest_plugins})
  add_subdirectory(${test_plugin})
endforeach()
