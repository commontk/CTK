
# Copy the xs3p.xsl stylesheet which is used in .xsd files to generate nice
# html output when opened in a browser.
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/xs3p.xsl
                                                              ${CTK_BINARY_DIR}/Documentation/html/xs3p.xsl)

if (BUILD_TESTING AND
    CTK_LIB_CommandLineModules/Core AND CTK_LIB_CommandLineModules/Backend/LocalProcess)

  # Compile source code snippets
  add_subdirectory(Snippets)
endif()
