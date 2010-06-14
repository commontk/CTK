PROJECT(CTKCore)

# CMake modules
SET(CMAKE_MODULE_PATH ${CTKCore_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# CMake Macros
INCLUDE(CMake/ctkMacroBFDCheck.cmake)

FIND_PACKAGE(Log4Qt)
if (NOT Log4Qt_FOUND)
  MESSAGE(FATAL_ERROR "error: Log4Qt package is required to build ${PROJECT_NAME}" )
endif()
#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

SET(KIT_export_directive "CTK_CORE_EXPORT")

# Additional directories to include
SET(KIT_include_directories
  ${Log4Qt_INCLUDE_DIR}
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
  ctkCommandLineParser.cpp
  ctkCommandLineParser.h
  ctkDependencyGraph.cpp
  ctkDependencyGraph.h
  ctkLogger.cpp
  ctkLogger.h
  ctkHistogram.cpp
  ctkHistogram.h
  ctkModelTester.cpp
  ctkModelTester.h
  ctkPimpl.h
  ctkSingleton.h
  ctkTransferFunction.cpp
  ctkTransferFunction.h
  ctkUtils.cpp
  ctkUtils.h
  )

IF(CTK_HAVE_BFD)
  LIST(APPEND KIT_SRCS
    ctkBinaryFileDescriptor.cpp
    ctkBinaryFileDescriptor.h
    )
ENDIF()

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkLogger.h 
  ctkHistogram.h
  ctkModelTester.h
  ctkTransferFunction.h
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
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
