PROJECT(ctkSimplePythonShell)

#
# 3rd party dependencies
#
INCLUDE(${VTK_USE_FILE})

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

SET(KIT_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/ctkSimplePythonManager.cpp
  ctkSimplePythonManager.h
  ctkSimplePythonQtDecorators.h
  ctkSimplePythonShellMain.cpp
  ctkTestWrappedQProperty.h
  ctkTestWrappedQInvokable.h
  ctkTestWrappedSlot.h
)

IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  LIST(APPEND KIT_SRCS
    ctkTestWrappedQListOfVTKObject.h
    ctkTestWrappedVTKSlot.h
    ctkTestWrappedVTKQInvokable.h
    )
ENDIF()

# Headers that should run through moc
SET(KIT_MOC_SRCS
  ctkSimplePythonManager.h
  ctkSimplePythonQtDecorators.h
  ctkTestWrappedQProperty.h
  ctkTestWrappedQInvokable.h
  ctkTestWrappedSlot.h
  )

IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  LIST(APPEND KIT_MOC_SRCS
    ctkTestWrappedQListOfVTKObject.h
    ctkTestWrappedVTKSlot.h
    ctkTestWrappedVTKQInvokable.h
    )
ENDIF()

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Set VTK_LIBRARIES variable
SET(VTK_LIBRARIES
  vtkCommon
  vtkFiltering
  )

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

# Configure file will be configured using CTK_WRAP_PYTHONQT_{LIGHT, FULL} variable
CONFIGURE_FILE(
  ctkSimplePythonShellConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkSimplePythonShellConfigure.h
  )
  
IF(CTK_WRAP_PYTHONQT_LIGHT OR CTK_WRAP_PYTHONQT_FULL)

  # Update list of target libraries with the list of available PythonQt libraries
  # and set variables holding list of pythonqt initialization method
  SET(CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION)
  SET(CTK_PYTHONQT_INITIALIZATION_METHOD_CALL)

  FOREACH(lib ${CTK_LIBS})
    ctkFunctionExtractOptionNameAndValue(${lib} lib_name lib_value)
    IF(${CTK_LIB_${lib_name}})
      STRING(REPLACE "/" "" lib_name_no_slash ${lib_name})

      LIST(APPEND KIT_target_libraries CTK${lib_name_no_slash}PythonQt)

      SET(CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION
        "${CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION}
void PythonQt_init_org_commontk_CTK${lib_name_no_slash}(PyObject*);")

      SET(CTK_PYTHONQT_INITIALIZATION_METHOD_CALL
        "${CTK_PYTHONQT_INITIALIZATION_METHOD_CALL}
PythonQt_init_org_commontk_CTK${lib_name_no_slash}(0);")
    ENDIF()
  ENDFOREACH()
ENDIF()

CONFIGURE_FILE(
  ctkSimplePythonManager.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkSimplePythonManager.cpp
  )

ctkMacroBuildApp(
  NAME ${PROJECT_NAME}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  )

ADD_SUBDIRECTORY(Python)

# Testing
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
