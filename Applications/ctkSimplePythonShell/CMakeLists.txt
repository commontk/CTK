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

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

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
  
  # To avoid the complex process of resolving the dependencies associated with each 
  # external project required by each decorator. For now, let's just include the decorator 
  # associated with CTKCore and CTKWidgets.
  SET(MY_CTK_WRAPPED_LIBRARIES_PYTHONQT)
  IF(CTK_LIB_Core)
    LIST(APPEND MY_CTK_WRAPPED_LIBRARIES_PYTHONQT CTKCore)
  ENDIF()
  IF(CTK_LIB_Widgets)
    LIST(APPEND MY_CTK_WRAPPED_LIBRARIES_PYTHONQT CTKWidgets)
  ENDIF()
  
  FOREACH(lib_name ${MY_CTK_WRAPPED_LIBRARIES_PYTHONQT})

    LIST(APPEND KIT_target_libraries ${lib_name}PythonQt)

    SET(CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION
      "${CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION}
void PythonQt_init_org_commontk_${lib_name}(PyObject*);")

    SET(CTK_PYTHONQT_INITIALIZATION_METHOD_CALL
      "${CTK_PYTHONQT_INITIALIZATION_METHOD_CALL}
PythonQt_init_org_commontk_${lib_name}(0);")

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
