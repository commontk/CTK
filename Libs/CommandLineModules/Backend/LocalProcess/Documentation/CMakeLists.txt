
# Copy XML schema file to the Doxygen output directory
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../Resources/ctkCmdLineModuleProcess.xsd
                                                              ${CTK_BINARY_DIR}/Documentation/html/ctkCmdLineModuleProcess.xsd)

