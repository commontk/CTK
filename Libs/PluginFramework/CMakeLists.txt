PROJECT(CTKPluginFramework)

#
# 3rd party dependencies
#


# use the QtMobility SuperBuild paths for now
# we should add a FindQtMobility later
SET(QTMOBILITY_INCLUDE_DIRS 
  "${CTK_BINARY_DIR}/../CMakeExternals/Source/QtMobility/install/include"
  )
SET(QTMOBILITY_LIBRARY_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
FIND_LIBRARY(QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG QtServiceFrameworkd
             PATHS ${QTMOBILITY_LIBRARY_DIR}
             )
             
FIND_LIBRARY(QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE QtServiceFramework
             PATHS ${QTMOBILITY_LIBRARY_DIR}
             )
             
SET(QTMOBILITY_QTSERVICEFW_LIBRARIES )
IF(QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE AND QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG)
  SET(QTMOBILITY_QTSERVICEFW_LIBRARIES optimized ${QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE} debug ${QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG})
ELSEIF(QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG)
  SET(QTMOBILITY_QTSERVICEFW_LIBRARIES ${QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG})
ELSEIF(QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE)
  SET(QTMOBILITY_QTSERVICEFW_LIBRARIES ${QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE})
ENDIF()

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_PLUGINFW_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  ${QTMOBILITY_INCLUDE_DIRS}
  )
  
# Source files
SET(KIT_SRCS
  ctkLDAPSearchFilter.cpp
  ctkPlugin.cpp
  ctkPluginArchive.cpp
  ctkPluginConstants.cpp
  ctkPluginContext.cpp
  ctkPluginDatabase.cpp
  ctkPluginDatabaseException.cpp
  ctkPluginEvent.cpp
  ctkPluginException.cpp
  ctkPluginFramework.cpp
  ctkPluginFrameworkFactory.cpp
  ctkPluginFrameworkContext.cpp
  ctkPluginFrameworkEvent.cpp
  ctkPluginFrameworkListeners.cpp
  ctkPluginFrameworkPrivate.cpp
  ctkPluginFrameworkUtil.cpp
  ctkPluginManager.cpp
  ctkPluginManifest.cpp
  ctkPluginPrivate.cpp
  ctkPlugins.cpp
  ctkQtServiceRegistration.cpp
  ctkQtServiceRegistrationPrivate.cpp
  ctkRequirePlugin.cpp
  ctkServiceException.cpp
  ctkServiceReference.cpp
  ctkServiceReferencePrivate.cpp
  ctkServiceRegistration.cpp
  ctkServiceRegistrationPrivate.cpp
  ctkServices.cpp
  ctkPluginStorage.cpp
  ctkVersion.cpp
  ctkVersionRange.cpp

  # EventBus sources
  EventBus/ctkEvent.cpp
  EventBus/ctkEventConstants.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkPluginEvent.h
  ctkPluginFrameworkEvent.h
  ctkPluginFrameworkListeners_p.h
  ctkServiceFactory.h
)

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Create a MANIFEST.MF resource for the PluginFramework library,
# pretending that is is a plugin (the system plugin)
ctkMacroGeneratePluginManifest(KIT_SRCS
  SYMBOLIC_NAME "system.plugin"
  VERSION "0.9.9"
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

# Testing
#IF(BUILD_TESTING)
#  ADD_SUBDIRECTORY(Testing)
#ENDIF(BUILD_TESTING)
