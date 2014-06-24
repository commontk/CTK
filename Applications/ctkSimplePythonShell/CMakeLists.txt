project(ctkSimplePythonShell)

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#

set(KIT_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/ctkSimplePythonManager.cpp
  ctkSimplePythonManager.h
  ctkSimplePythonQtDecorators.h
  ctkSimplePythonShellMain.cpp
  ctkTestWrappedQProperty.h
  ctkTestWrappedQInvokable.h
  ctkTestWrappedSlot.h
)

if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  list(APPEND KIT_SRCS
    ctkTestWrappedQListOfVTKObject.h
    ctkTestWrappedVTKObserver.h
    ctkTestWrappedVTKQInvokable.h
    ctkTestWrappedVTKSlot.h
    )
endif()

# Headers that should run through moc
set(KIT_MOC_SRCS
  ctkSimplePythonManager.h
  ctkSimplePythonQtDecorators.h
  ctkTestWrappedQProperty.h
  ctkTestWrappedQInvokable.h
  ctkTestWrappedSlot.h
  )

if(CTK_LIB_Scripting/Python/Core AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  list(APPEND KIT_MOC_SRCS
    ctkTestWrappedQListOfVTKObject.h
    ctkTestWrappedVTKObserver.h
    ctkTestWrappedVTKQInvokable.h
    ctkTestWrappedVTKSlot.h
    )
endif()

# UI files
set(KIT_UI_FORMS
  )

# Resources
set(KIT_resources
  )

set(VTK_LIBRARIES
  )

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

# Configure file will be configured using CTK_WRAP_PYTHONQT_LIGHT and
# CTK_BUILD_SHARED_LIBS variables
configure_file(
  ctkSimplePythonShellConfigure.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkSimplePythonShellConfigure.h
  )

# The following block generate the code required to initialize the wrapper
# when CTK is statically built.
if(NOT CTK_BUILD_SHARED_LIBS AND CTK_WRAP_PYTHONQT_LIGHT)

  # Update list of target libraries with the list of available PythonQt libraries
  # and set variables holding list of pythonqt initialization method
  set(CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION)
  set(CTK_PYTHONQT_INITIALIZATION_METHOD_CALL)

  # To avoid the complex process of resolving the dependencies associated with each
  # external project required by each decorator. For now, let's just include the decorator
  # associated with CTKCore and CTKWidgets.
  set(MY_CTK_WRAPPED_LIBRARIES_PYTHONQT)
  if(CTK_LIB_Core)
    list(APPEND MY_CTK_WRAPPED_LIBRARIES_PYTHONQT CTKCore)
  endif()
  if(CTK_LIB_Widgets)
    list(APPEND MY_CTK_WRAPPED_LIBRARIES_PYTHONQT CTKWidgets)
  endif()
  if(CTK_LIB_DICOM/Core)
    list(APPEND MY_CTK_WRAPPED_LIBRARIES_PYTHONQT CTKDICOMCore)
  endif()
  if(CTK_LIB_DICOM/Widgets)
    list(APPEND MY_CTK_WRAPPED_LIBRARIES_PYTHONQT CTKDICOMWidgets)
  endif()

  foreach(lib_name ${MY_CTK_WRAPPED_LIBRARIES_PYTHONQT})

    list(APPEND KIT_target_libraries ${lib_name}PythonQt)

    set(CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION
      "${CTK_PYTHONQT_INITIALIZATION_METHOD_DEFINITION}
extern \"C\"{ void init${lib_name}PythonQt(); }")

    set(CTK_PYTHONQT_INITIALIZATION_METHOD_CALL
      "${CTK_PYTHONQT_INITIALIZATION_METHOD_CALL}
init${lib_name}PythonQt();")

  endforeach()
endif()

configure_file(
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

add_subdirectory(Python)

# Testing
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
