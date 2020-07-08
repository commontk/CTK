project(CTKCore)

#
# 3rd party dependencies
#

# CMake modules
set(CMAKE_MODULE_PATH ${CTKCore_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# CMake Macros
include(CMake/ctkMacroBFDCheck.cmake) # HAVE_BFD will be set to True if it applies

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_CORE_EXPORT")

# Source files
set(KIT_SRCS
  ctkAbstractFactory.h
  ctkAbstractFactory.tpp
  ctkAbstractFileBasedFactory.h
  ctkAbstractFileBasedFactory.tpp
  ctkAbstractObjectFactory.h
  ctkAbstractObjectFactory.tpp
  ctkAbstractPluginFactory.h
  ctkAbstractPluginFactory.tpp
  ctkAbstractQObjectFactory.h
  ctkAbstractQObjectFactory.tpp
  ctkAbstractLibraryFactory.h
  ctkAbstractLibraryFactory.tpp
  ctkBackTrace.cpp
  ctkBooleanMapper.cpp
  ctkBooleanMapper.h
  ctkCallback.cpp
  ctkCallback.h
  ctkCommandLineParser.cpp
  ctkCommandLineParser.h
  ctkCoreSettings.cpp
  ctkCoreSettings.h
  ctkCoreTestingUtilities.cpp
  ctkCoreTestingUtilities.tpp
  ctkCoreTestingUtilities.h
  ctkDependencyGraph.cpp
  ctkDependencyGraph.h
  ctkErrorLogAbstractMessageHandler.cpp
  ctkErrorLogAbstractMessageHandler.h
  ctkErrorLogAbstractModel.cpp
  ctkErrorLogAbstractModel.h
  ctkErrorLogContext.h
  ctkErrorLogFDMessageHandler.cpp
  ctkErrorLogFDMessageHandler.h
  ctkErrorLogFDMessageHandler_p.h
  ctkErrorLogLevel.cpp
  ctkErrorLogLevel.h
  ctkErrorLogQtMessageHandler.cpp
  ctkErrorLogQtMessageHandler.h
  ctkErrorLogStreamMessageHandler.cpp
  ctkErrorLogStreamMessageHandler.h
  ctkErrorLogTerminalOutput.cpp
  ctkErrorLogTerminalOutput.h
  ctkException.cpp
  ctkException.h
  ctkFileLogger.cpp
  ctkFileLogger.h
  ctkHighPrecisionTimer.cpp
  ctkLinearValueProxy.cpp
  ctkLinearValueProxy.h
  ctkLogger.cpp
  ctkLogger.h
  ctkModelTester.cpp
  ctkModelTester.h
  ctkPimpl.h
  ctkScopedCurrentDir.cpp
  ctkScopedCurrentDir.h
  ctkSingleton.h
  ctkUtils.cpp
  ctkUtils.h
  ctkValueProxy.cpp
  ctkValueProxy.h
  ctkWorkflow.h
  ctkWorkflow.cpp
  ctkWorkflow_p.h
  ctkWorkflowStep.h
  ctkWorkflowStep.cpp
  ctkWorkflowStep_p.h
  ctkWorkflowTransitions.h
  ctkSetName.cpp
  ctkSetName.h
  )

if(HAVE_BFD)
  list(APPEND KIT_SRCS
    ctkBinaryFileDescriptor.cpp
    ctkBinaryFileDescriptor.h
    )
endif()

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkBooleanMapper.h
  ctkCallback.h
  ctkCommandLineParser.h
  ctkCoreSettings.h
  ctkErrorLogAbstractMessageHandler.h
  ctkErrorLogAbstractModel.h
  ctkErrorLogFDMessageHandler_p.h
  ctkErrorLogLevel.h
  ctkErrorLogQtMessageHandler.h
  ctkErrorLogTerminalOutput.h
  ctkFileLogger.h
  ctkLinearValueProxy.h
  ctkLogger.h
  ctkModelTester.h
  ctkErrorLogQtMessageHandler.h
  ctkValueProxy.h
  ctkWorkflow.h
  ctkWorkflow_p.h
  ctkWorkflowStep_p.h
  ctkWorkflowTransitions.h
  )

# UI files
set(KIT_UI_FORMS
)

# Resources
set(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
set(KIT_target_libraries)

ctkFunctionGetTargetLibraries(KIT_target_libraries)

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::Core)
endif()

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

# Needed for ctkBackTrace
if(UNIX)
  # FreeBSD: The same functionality that is in linux's libdl is provided in FreeBSD's libc
  find_library(HAVE_LIBDL dl)
  if(HAVE_LIBDL)
    target_link_libraries(${PROJECT_NAME} dl)
  endif()
elseif(WIN32 AND NOT MINGW)
  target_link_libraries(${PROJECT_NAME} dbghelp)
endif()

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()


# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)

  # Compile source code snippets
  add_subdirectory(Documentation/Snippets)
endif()
