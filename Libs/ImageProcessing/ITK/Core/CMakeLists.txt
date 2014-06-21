project(CTKImageProcessingITKCore)

#
# 3rd party dependencies
#

if(ITK_VERSION_MAJOR GREATER 3)
  #
  # ITK
  #
  set(ITK_FOUND 0) # XXX Ideally every library should 'find_package(ITK ...)" and the
                   #     top-level call to find_package(ITK ...) should be removed.
  set(${PROJECT_NAME}_ITK_COMPONENTS
    ITKCommon
    )
  find_package(ITK 4 COMPONENTS ${${PROJECT_NAME}_ITK_COMPONENTS} REQUIRED)
  set(ITK_NO_IO_FACTORY_REGISTER_MANAGER 1)
endif()
include(${ITK_USE_FILE})

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_IMAGEPROCESSING_ITK_CORE_EXPORT")


# Source files
set(KIT_SRCS
  ctkITKErrorLogMessageHandler.cpp
  ctkITKErrorLogMessageHandler.h
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Plugins
#if(CTK_BUILD_QTDESIGNER_PLUGINS)
#  add_subdirectory(Plugins)
#endif()

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
