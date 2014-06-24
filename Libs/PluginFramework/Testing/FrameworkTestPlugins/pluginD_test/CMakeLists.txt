project(pluginD_test)

set(PLUGIN_export_directive "pluginD_test_EXPORT")

set(PLUGIN_SRCS
  ctkTestPluginD.cpp
)

set(lib_name ${PROJECT_NAME})

if(CTK_QT_VERSION VERSION_LESS "5")
  include(${QT_USE_FILE})
endif()

add_library(${lib_name} SHARED ${PLUGIN_SRCS})

if(CTK_QT_VERSION VERSION_LESS "5")
  target_link_libraries(${lib_name} ${QT_LIBRARIES})
else()
  target_link_libraries(${lib_name} Qt5::Core)
endif()

# Apply properties to the library target.
set_target_properties(${lib_name} PROPERTIES
#    COMPILE_FLAGS "-DQT_PLUGIN"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_plugins
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test_plugins
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test_plugins
    PREFIX "lib"
    )
