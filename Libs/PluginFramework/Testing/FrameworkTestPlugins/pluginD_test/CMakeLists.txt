PROJECT(pluginD_test)

SET(PLUGIN_export_directive "pluginD_test_EXPORT")

SET(PLUGIN_SRCS
  ctkTestPluginD.cpp
)

SET(lib_name ${PROJECT_NAME})

INCLUDE(${QT_USE_FILE})

ADD_LIBRARY(${lib_name} SHARED ${PLUGIN_SRCS})
TARGET_LINK_LIBRARIES(${lib_name} ${QT_LIBRARIES})

# Set the output directory for the plugin
SET(output_dir_suffix "test_plugins")
IF(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  SET(runtime_output_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${output_dir_suffix}")
ELSE()
  SET(runtime_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${output_dir_suffix}")
ENDIF()
IF(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  SET(library_output_dir "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${output_dir_suffix}")
ELSE()
  SET(library_output_dir "${CMAKE_CURRENT_BINARY_DIR}/${output_dir_suffix}")
ENDIF()

# Apply properties to the library target.
SET_TARGET_PROPERTIES(${lib_name} PROPERTIES
#    COMPILE_FLAGS "-DQT_PLUGIN"
    RUNTIME_OUTPUT_DIRECTORY ${runtime_output_dir}
    LIBRARY_OUTPUT_DIRECTORY ${library_output_dir}
    PREFIX "lib"
    )
