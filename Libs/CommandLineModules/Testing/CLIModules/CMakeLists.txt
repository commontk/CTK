
# This is very simple and for test purposes
# only. Relies on naming conventions and has
# no extensive error checking yet.
function(ctkFunctionCreateCLIModule name)
  set(_src_files ${ARGN})
  list(APPEND _src_files ctkCLIModule${name}.cpp)
  qt4_add_resources(_src_files ctkCLIModule${name}.qrc)

  add_executable(ctkCLIModule${name} ${_src_files})
  target_link_libraries(ctkCLIModule${name} CTKCore ${QT_LIBRARIES})
  add_dependencies(ctkCLITestModules ctkCLIModule${name})
endfunction()

set(_cli_modules
  Blur2dImage
  Tour
)

add_custom_target(ctkCLITestModules)

foreach(_cli_module ${_cli_modules})
  add_subdirectory(${_cli_module})
endforeach()
