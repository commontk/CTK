
# This is very simple and for test purposes
# only. Relies on naming conventions and has
# no extensive error checking yet.
function(ctkFunctionCreateCmdLineModule name)
  set(_src_files ${ARGN})
  list(APPEND _src_files ctkCmdLineModule${name}.cpp)
  if(CTK_QT_VERSION VERSION_LESS "5")
    qt4_add_resources(_src_files ctkCmdLineModule${name}.qrc)
  else()
    qt5_add_resources(_src_files ctkCmdLineModule${name}.qrc)
  endif()

  ctk_add_executable_utf8(ctkCmdLineModule${name} ${_src_files})
  if(CTK_QT_VERSION VERSION_LESS "5")
    target_link_libraries(ctkCmdLineModule${name} CTKCore ${QT_LIBRARIES})
  else()
    target_link_libraries(ctkCmdLineModule${name} CTKCore Qt5::Widgets)
  endif()
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
