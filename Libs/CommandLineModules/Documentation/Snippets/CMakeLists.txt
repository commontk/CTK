
set(_base_src_include_dir ${CMAKE_SOURCE_DIR}/Libs/CommandLineModules)
set(_base_bin_include_dir ${CMAKE_BINARY_DIR}/Libs/CommandLineModules)

include_directories(
  ${CTKCore_SOURCE_DIR}
  ${CTKCore_BINARY_DIR}
  ${_base_src_include_dir}/Core
  ${_base_bin_include_dir}/Core
  ${_base_src_include_dir}/Backend/LocalProcess
  ${_base_bin_include_dir}/Backend/LocalProcess
  ${_base_src_include_dir}/Frontend/QtGui
  ${_base_bin_include_dir}/Frontend/QtGui
)

ctkFunctionCompileSnippets("${CMAKE_CURRENT_SOURCE_DIR}"
  CTKCommandLineModulesBackendLocalProcess
  CTKCommandLineModulesFrontendQtGui
)
