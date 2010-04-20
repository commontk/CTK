PROJECT(CTKCore)

#
# 3rd party dependencies
#

# use the QtMobility SuperBuild paths for now
# we should add a FindQtMobility later
SET(QTMOBILITY_INCLUDE_DIRS 
  "${CTK_BINARY_DIR}/../CMakeExternals/Source/QtMobility/install/include"
  )
SET(QTMOBILITY_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
FIND_LIBRARY(QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG QtServiceFrameworkd
             PATHS ${QTMOBILITY_LIBRARY_DIR}
             )
             
FIND_LIBRARY(QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE QtServiceFramework
             PATHS ${QTMOBILITY_LIBRARY_DIR}
             )
             
SET(QTMOBILITY_QTSERVICEFW_LIBRARIES )
IF(QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE)
  LIST(APPEND QTMOBILITY_QTSERVICEFW_LIBRARIES optimized ${QTMOBILITY_QTSERVICEFW_LIBRARY_RELEASE})
ENDIF()
IF(QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG)
  LIST(APPEND QTMOBILITY_QTSERVICEFW_LIBRARIES debug ${QTMOBILITY_QTSERVICEFW_LIBRARY_DEBUG})
ENDIF()
             
#
# See CTK/CMake/ctkMacroBuildQtLib.cmake for details
#

SET(KIT_export_directive "CTK_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  ${QTMOBILITY_INCLUDE_DIRS}
  )
  
# Source files
SET(KIT_SRCS
  ctkAbstractFactory.h
  ctkAbstractFactory.tpp
  ctkAbstractObjectFactory.h
  ctkAbstractObjectFactory.tpp
  ctkAbstractPluginFactory.h
  ctkAbstractPluginFactory.tpp
  ctkAbstractQObjectFactory.h
  ctkAbstractQObjectFactory.tpp
  ctkAbstractLibraryFactory.h
  ctkAbstractLibraryFactory.tpp
  ctkDependencyGraph.cpp
  ctkDependencyGraph.h
  ctkModelTester.cpp
  ctkModelTester.h
  ctkPimpl.h
  ctkUtils.cpp
  ctkUtils.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkModelTester.h
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildQtLib(
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

# Plugins
#ADD_SUBDIRECTORY(Plugins)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
