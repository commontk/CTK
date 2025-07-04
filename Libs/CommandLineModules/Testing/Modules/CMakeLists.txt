
# This is very simple and for test purposes
# only. Relies on naming conventions and has
# no extensive error checking yet.
function(ctkFunctionCreateCmdLineModule name)
  set(_src_files ${ARGN})
  list(APPEND _src_files ctkCmdLineModule${name}.cpp)

  ctk_add_executable_utf8(ctkCmdLineModule${name} ${_src_files})

  target_link_libraries(ctkCmdLineModule${name} CTKCore Qt${CTK_QT_VERSION}::Widgets)

  add_dependencies(ctkCmdLineTestModules ctkCmdLineModule${name})
endfunction()

set(_cmdline_modules
  Blur2dImage
  TestBed
  Tour
)

add_custom_target(ctkCmdLineTestModules)

foreach(_cmdline_module ${_cmdline_modules})
  add_subdirectory(${_cmdline_module})
endforeach()
