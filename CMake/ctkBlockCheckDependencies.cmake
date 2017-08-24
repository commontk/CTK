
#
# Independently of the value of CTK_SUPERBUILD, each external project definition will
# provides either the include and library directories or a variable name
# used by the corresponding Find<package>.cmake files.
#
# Within top-level CMakeLists.txt file, the variable names will be expanded if not in
# superbuild mode. The include and library dirs are then used in
# ctkMacroBuildApp, ctkMacroBuildLib, and ctkMacroBuildPlugin
#

#-----------------------------------------------------------------------------
# Collect CTK library target dependencies
#

ctkMacroCollectAllTargetLibraries("${CTK_LIBS}" "Libs" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_PLUGINS}" "Plugins" ALL_TARGET_LIBRARIES)
ctkMacroCollectAllTargetLibraries("${CTK_APPS}" "Applications" ALL_TARGET_LIBRARIES)
#message(STATUS ALL_TARGET_LIBRARIES:${ALL_TARGET_LIBRARIES})

#-----------------------------------------------------------------------------
# Initialize NON_CTK_DEPENDENCIES variable
#
# Using the variable ALL_TARGET_LIBRARIES initialized above with the help
# of the macro ctkMacroCollectAllTargetLibraries, let's get the list of all Non-CTK dependencies.
# NON_CTK_DEPENDENCIES is expected by the macro ctkMacroShouldAddExternalProject
ctkMacroGetAllNonProjectTargetLibraries("${ALL_TARGET_LIBRARIES}" NON_CTK_DEPENDENCIES)
#message(NON_CTK_DEPENDENCIES:${NON_CTK_DEPENDENCIES})

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#

if(CTK_SUPERBUILD)
  set(ep_install_dir ${CMAKE_BINARY_DIR}/CMakeExternals/Install)
  set(ep_suffix      "-cmake")

  set(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
  set(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")

  set(ep_cxx_standard_arg)
  if(CMAKE_CXX_STANDARD)
    set(ep_cxx_standard_arg "-DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}")
  endif()

  set(ep_common_cache_args
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
      -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
      -DBUILD_TESTING:BOOL=OFF
      -DCMAKE_MACOSX_RPATH:BOOL=${CMAKE_MACOSX_RPATH}
      ${ep_cxx_standard_arg}
     )
endif()

if(NOT DEFINED CTK_DEPENDENCIES)
  message(FATAL_ERROR "error: CTK_DEPENDENCIES variable is not defined !")
endif()

set(DCMTK_enabling_variable DCMTK_LIBRARIES)
set(${DCMTK_enabling_variable}_INCLUDE_DIRS DCMTK_INCLUDE_DIR)
set(${DCMTK_enabling_variable}_FIND_PACKAGE_CMD DCMTK)

set(ITK_enabling_variable ITK_LIBRARIES)
set(${ITK_enabling_variable}_LIBRARY_DIRS ITK_LIBRARY_DIRS)
set(${ITK_enabling_variable}_INCLUDE_DIRS ITK_INCLUDE_DIRS)
set(${ITK_enabling_variable}_FIND_PACKAGE_CMD ITK)

set(Log4Qt_enabling_variable Log4Qt_LIBRARIES)
set(${Log4Qt_enabling_variable}_INCLUDE_DIRS Log4Qt_INCLUDE_DIRS)
set(${Log4Qt_enabling_variable}_FIND_PACKAGE_CMD Log4Qt)

set(OpenIGTLink_enabling_variable OpenIGTLink_LIBRARIES)
set(${OpenIGTLink_enabling_variable}_LIBRARY_DIRS OpenIGTLink_LIBRARY_DIRS)
set(${OpenIGTLink_enabling_variable}_INCLUDE_DIRS OpenIGTLink_INCLUDE_DIRS)
set(${OpenIGTLink_enabling_variable}_FIND_PACKAGE_CMD OpenIGTLink)

set(PythonQt_enabling_variable PYTHONQT_LIBRARIES)
set(${PythonQt_enabling_variable}_INCLUDE_DIRS PYTHONQT_INCLUDE_DIR PYTHON_INCLUDE_DIRS)
set(${PythonQt_enabling_variable}_FIND_PACKAGE_CMD PythonQt)

set(QtSOAP_enabling_variable QtSOAP_LIBRARIES)
set(${QtSOAP_enabling_variable}_LIBRARY_DIRS QtSOAP_LIBRARY_DIRS)
set(${QtSOAP_enabling_variable}_INCLUDE_DIRS QtSOAP_INCLUDE_DIRS)
set(${QtSOAP_enabling_variable}_FIND_PACKAGE_CMD QtSOAP)

set(QtTesting_enabling_variable QtTesting_LIBRARIES)
set(${QtTesting_enabling_variable}_INCLUDE_DIRS QtTesting_INCLUDE_DIRS)
set(${QtTesting_enabling_variable}_FIND_PACKAGE_CMD QtTesting)

set(qxmlrpc_enabling_variable qxmlrpc_LIBRARY)
set(${qxmlrpc_enabling_variable}_LIBRARY_DIRS qxmlrpc_LIBRARY_DIRS)
set(${qxmlrpc_enabling_variable}_INCLUDE_DIRS qxmlrpc_INCLUDE_DIRS)
set(${qxmlrpc_enabling_variable}_FIND_PACKAGE_CMD qxmlrpc)

set(qRestAPI_enabling_variable qRestAPI_LIBRARIES)
set(${qRestAPI_enabling_variable}_LIBRARY_DIRS qRestAPI_LIBRARY_DIRS)
set(${qRestAPI_enabling_variable}_INCLUDE_DIRS qRestAPI_INCLUDE_DIRS)
set(${qRestAPI_enabling_variable}_FIND_PACKAGE_CMD qRestAPI)

set(VTK_enabling_variable VTK_LIBRARIES)
set(${VTK_enabling_variable}_LIBRARY_DIRS VTK_LIBRARY_DIRS)
set(${VTK_enabling_variable}_INCLUDE_DIRS VTK_INCLUDE_DIRS)
set(${VTK_enabling_variable}_FIND_PACKAGE_CMD VTK)

set(ZMQ_enabling_variable ZMQ_LIBRARIES)
set(${ZMQ_enabling_variable}_LIBRARY_DIRS ZMQ_LIBRARY_DIRS)
set(${ZMQ_enabling_variable}_INCLUDE_DIRS ZMQ_INCLUDE_DIRS)
set(${ZMQ_enabling_variable}_FIND_PACKAGE_CMD ZMQ)

macro(superbuild_is_external_project_includable possible_proj output_var)
  if(DEFINED ${possible_proj}_enabling_variable)
    ctkMacroShouldAddExternalProject(${${possible_proj}_enabling_variable} ${output_var})
    if(NOT ${${output_var}})
      if(${possible_proj} STREQUAL "VTK"
         AND CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
        set(${output_var} 1)
      else()
        unset(${${possible_proj}_enabling_variable}_INCLUDE_DIRS)
        unset(${${possible_proj}_enabling_variable}_LIBRARY_DIRS)
        unset(${${possible_proj}_enabling_variable}_FIND_PACKAGE_CMD)
      endif()
    endif()
  else()
    set(${output_var} 1)
  endif()
endmacro()

set(proj CTK)
ExternalProject_Include_Dependencies(CTK
  PROJECT_VAR proj
  DEPENDS_VAR CTK_DEPENDENCIES
  USE_SYSTEM_VAR ${CMAKE_PROJECT_NAME}_USE_SYSTEM_${proj}
  )

#message("Updated CTK_DEPENDENCIES:")
#foreach(dep ${CTK_DEPENDENCIES})
#  message("  ${dep}")
#endforeach()
