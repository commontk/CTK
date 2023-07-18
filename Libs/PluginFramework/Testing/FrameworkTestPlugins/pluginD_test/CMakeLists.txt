project(pluginD_test)

set(PLUGIN_export_directive "pluginD_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginD.cpp
)

set(lib_name ${PROJECT_NAME})

add_library(${lib_name} SHARED ${PLUGIN_SRCS})

target_link_libraries(${lib_name} Qt${CTK_QT_VERSION}::Core)

# Apply properties to the library target.
set_target_properties(${lib_name} PROPERTIES
#    COMPILE_FLAGS "-DQT_PLUGIN"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_plugins
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test_plugins
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test_plugins
    PREFIX "lib"
    )
