###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#
# Depends on:
#  CTK/CMake/ctkMacroParseArguments.cmake
#


#! When CTK is built as shared library, the following macro builds a python module
#! associated with the generated PythonQt wrappers. When loaded, it will
#! dynamically loads both the (1) generated decorators and the (2) hand written one.
#! On the other hand, when CTK is built statically, it creates a
#! static library providing a initialization function that will allow to load
#! both (1) and (2).

#! \ingroup CMakeAPI
MACRO(ctkMacroBuildLibWrapper)
  ctkMacroParseArguments(MY
    "NAMESPACE;TARGET;SRCS;WRAPPER_LIBRARY_TYPE;ARCHIVE_OUTPUT_DIRECTORY;LIBRARY_OUTPUT_DIRECTORY;RUNTIME_OUTPUT_DIRECTORY;INSTALL_BIN_DIR;INSTALL_LIB_DIR"
    "NO_INSTALL"
    ${ARGN}
    )

  # Sanity checks
  IF(NOT DEFINED MY_TARGET)
    MESSAGE(FATAL_ERROR "NAME is mandatory")
  ENDIF()
  IF(NOT DEFINED MY_WRAPPER_LIBRARY_TYPE OR "${MY_WRAPPER_LIBRARY_TYPE}" STREQUAL "SHARED")
    SET(MY_WRAPPER_LIBRARY_TYPE "MODULE")
  ENDIF()

  IF(NOT DEFINED MY_NAMESPACE)
    SET(MY_NAMESPACE "org.commontk")
  ENDIF()
  IF(NOT DEFINED CTK_WRAP_PYTHONQT_FULL)
    SET(CTK_WRAP_PYTHONQT_FULL FALSE)
  ENDIF()
  FOREACH(type RUNTIME LIBRARY ARCHIVE)
    IF(NOT DEFINED MY_${type}_OUTPUT_DIRECTORY)
      SET(MY_${type}_OUTPUT_DIRECTORY ${CMAKE_${type}_OUTPUT_DIRECTORY})
    ENDIF()
  ENDFOREACH()
  IF(NOT DEFINED MY_INSTALL_BIN_DIR)
    SET(MY_INSTALL_BIN_DIR ${CTK_INSTALL_BIN_DIR})
  ENDIF()
  IF(NOT DEFINED MY_INSTALL_LIB_DIR)
    SET(MY_INSTALL_LIB_DIR ${CTK_INSTALL_LIB_DIR})
  ENDIF()

  # Define library name
  SET(lib_name ${MY_TARGET})

  # Include dirs
  SET(my_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

  # Since the PythonQt decorator depends on PythonQt, Python and VTK, let's link against
  # these ones to avoid complaints of MSVC
  # Note: "LINK_DIRECTORIES" has to be invoked before "ADD_LIBRARY"
  SET(my_EXTRA_PYTHON_LIBRARIES ${PYTHON_LIBRARY} ${PYTHONQT_LIBRARIES})
  # Should we link against VTK
  #IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  #  LIST(APPEND my_EXTRA_PYTHON_LIBRARIES vtkCommon vtkPythonCore)
  #ENDIF()

  # The current library might not be wrapped. Nevertheless, if one of its dependent library
  # is linked using vtkCommon or vtkPythonCore, VTK_LIBRARY_DIRS should be added
  # as a link directories.
  #IF(NOT CTK_BUILD_SHARED_LIBS
  #   AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  #  LINK_DIRECTORIES(${VTK_LIBRARY_DIRS})
  #ENDIF()

  # Does a header having the expected filename exists ?
  STRING(REGEX REPLACE "^CTK" "ctk" lib_name_lc_ctk ${lib_name})
  SET(DECORATOR_HEADER ${lib_name_lc_ctk}PythonQtDecorators.h)
  SET(HAS_DECORATOR FALSE)
  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DECORATOR_HEADER})
    SET(HAS_DECORATOR TRUE)
    SET(DECORATOR_HEADER ${DECORATOR_HEADER})
    SET_SOURCE_FILES_PROPERTIES(${DECORATOR_HEADER} WRAP_EXCLUDE)
  ENDIF()
  #message("HAS_DECORATOR:${HAS_DECORATOR}")
  #message("path/to/DECORATOR_HEADER:${CMAKE_CURRENT_SOURCE_DIR}/${DECORATOR_HEADER}")

  SET(KIT_PYTHONQT_SRCS) # Clear variable
  ctkMacroWrapPythonQt(${MY_NAMESPACE} ${lib_name}
    KIT_PYTHONQT_SRCS "${MY_SRCS}" ${CTK_WRAP_PYTHONQT_FULL} ${HAS_DECORATOR})
  IF(HAS_DECORATOR)
    LIST(APPEND KIT_PYTHONQT_SRCS ${DECORATOR_HEADER})
    QT4_WRAP_CPP(KIT_PYTHONQT_SRCS ${DECORATOR_HEADER} OPTIONS -f${DECORATOR_HEADER})
  ENDIF()
  ADD_LIBRARY(${lib_name}PythonQt ${MY_WRAPPER_LIBRARY_TYPE} ${KIT_PYTHONQT_SRCS})
  TARGET_LINK_LIBRARIES(${lib_name}PythonQt ${lib_name} ${my_EXTRA_PYTHON_LIBRARIES})
  IF(MY_WRAPPER_LIBRARY_TYPE STREQUAL "STATIC")
    IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      SET_TARGET_PROPERTIES(${lib_name}PythonQt PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF()
  ENDIF()
  IF(MY_WRAPPER_LIBRARY_TYPE STREQUAL "MODULE")
    # Make sure that no prefix is set on the library
    set_target_properties(${lib_name}PythonQt PROPERTIES PREFIX "")
    # Python extension modules on Windows must have the extension ".pyd"
    # instead of ".dll" as of Python 2.5.  Older python versions do support
    # this suffix.
    # See http://docs.python.org/faq/windows.html#is-a-pyd-file-the-same-as-a-dll
    IF(WIN32 AND NOT CYGWIN)
      set_target_properties(${lib_name}PythonQt PROPERTIES SUFFIX ".pyd")
    ENDIF()
  ENDIF()
  set_target_properties(${lib_name}PythonQt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${MY_RUNTIME_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${MY_LIBRARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY "${MY_ARCHIVE_OUTPUT_DIRECTORY}"
    )

  # Set labels associated with the target.
  SET_TARGET_PROPERTIES(${lib_name}PythonQt PROPERTIES LABELS ${lib_name})

  # Update list of libraries wrapped with PythonQt
  SET(CTK_WRAPPED_LIBRARIES_PYTHONQT
    ${CTK_WRAPPED_LIBRARIES_PYTHONQT} ${lib_name}
    CACHE INTERNAL "CTK libraries wrapped using PythonQt" FORCE)

  # Install rules
  IF(NOT MY_NO_INSTALL AND MY_WRAPPER_LIBRARY_TYPE STREQUAL "MODULE")
    INSTALL(TARGETS ${lib_name}PythonQt
      RUNTIME DESTINATION ${MY_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
      LIBRARY DESTINATION ${MY_INSTALL_LIB_DIR} COMPONENT RuntimePlugins
      ARCHIVE DESTINATION ${MY_INSTALL_LIB_DIR} COMPONENT Development)
  ENDIF()

ENDMACRO()


