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
  ctkCheckableModelHelper.cpp
  ctkCheckableModelHelper.h
  ctkCommandLineParser.cpp
  ctkCommandLineParser.h
  ctkDependencyGraph.cpp
  ctkDependencyGraph.h
  ctkErrorLogModel.cpp
  ctkErrorLogModel.h
  ctkErrorLogFDMessageHandler.cpp
  ctkErrorLogFDMessageHandler.h
  ctkErrorLogFDMessageHandler_p.h
  ctkErrorLogQtMessageHandler.cpp
  ctkErrorLogQtMessageHandler.h
  ctkErrorLogStreamMessageHandler.cpp
  ctkErrorLogStreamMessageHandler.h
  ctkException.cpp
  ctkException.h
  ctkHighPrecisionTimer.cpp
  ctkLinearValueProxy.cpp
  ctkLinearValueProxy.h
  ctkLogger.cpp
  ctkLogger.h
  ctkHistogram.cpp
  ctkHistogram.h
  ctkModelTester.cpp
  ctkModelTester.h
  ctkPimpl.h
  ctkScopedCurrentDir.cpp
  ctkScopedCurrentDir.h
  ctkSingleton.h
  ctkTransferFunction.cpp
  ctkTransferFunction.h
  ctkTransferFunctionRepresentation.cpp
  ctkTransferFunctionRepresentation.h
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
  ctkCheckableModelHelper.h
  ctkCommandLineParser.h
  ctkErrorLogFDMessageHandler_p.h
  ctkErrorLogModel.h
  ctkLinearValueProxy.h
  ctkLogger.h
  ctkHistogram.h
  ctkModelTester.h
  ctkTransferFunction.h
  ctkTransferFunctionRepresentation.h
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
  target_link_libraries(${PROJECT_NAME} dl)
elseif(WIN32 AND NOT MINGW)
  target_link_libraries(${PROJECT_NAME} dbghelp)
endif()

if(CTK_WRAP_PYTHONQT_FULL OR CTK_WRAP_PYTHONQT_LIGHT)
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
