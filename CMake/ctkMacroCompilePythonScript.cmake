

#
# Based on ParaView/VTK/Utilities/vtkTclTest2Py/CMakeLists.txt and
#          ParaView/VTK/Wrapping/Python/CMakeLists.txt
#

#
# By globally defining the variable CTK_COMPILE_PYTHON_SCRIPTS_GLOBAL_TARGET_NAME to a
# non-empty string or by specifying the macro option 'GLOBAL_TARGET',
# the following targets will be defined for the whole build system:
#  - Copy<GLOBAL_TARGET_NAME>PythonResourceFiles
#  - Copy<GLOBAL_TARGET_NAME>PythonScriptFiles
#  - Compile<GLOBAL_TARGET_NAME>PythonFiles
#
# For complex projects, this can help reducing the number of targets and
# simplify the manual rebuild of copy and compile targets.
#

include(${CTK_CMAKE_DIR}/ctkMacroParseArguments.cmake)

set(CTK_PYTHON_COMPILE_FILE_SCRIPT_DIR "${CMAKE_BINARY_DIR}/CMakeFiles")

#! \ingroup CMakeAPI
macro(ctkMacroCompilePythonScript)
  ctkMacroParseArguments(MY
    "TARGET_NAME;SCRIPTS;RESOURCES;SOURCE_DIR;DESTINATION_DIR;INSTALL_DIR"
    "NO_INSTALL_SUBDIR;GLOBAL_TARGET"
    ${ARGN}
    )

  # Sanity checks
  foreach(varname TARGET_NAME SCRIPTS DESTINATION_DIR INSTALL_DIR)
    if(NOT DEFINED MY_${varname})
      message(FATAL_ERROR "${varname} is mandatory")
    endif()
  endforeach()

  if(NOT DEFINED MY_SOURCE_DIR)
    set(MY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if("${CTK_COMPILE_PYTHON_SCRIPTS_GLOBAL_TARGET_NAME}" STREQUAL "")
    set(target ${MY_TARGET_NAME})
  else()
    set(MY_GLOBAL_TARGET TRUE)
    set(target ${CTK_COMPILE_PYTHON_SCRIPTS_GLOBAL_TARGET_NAME})
  endif()

  # Since 'add_custom_command' doesn't play nicely with path having multiple
  # consecutive slashes. Let's make sure there are no trailing slashes.
  get_filename_component(MY_SOURCE_DIR ${MY_SOURCE_DIR} REALPATH)
  get_filename_component(MY_DESTINATION_DIR ${MY_DESTINATION_DIR} REALPATH)

  set(input_python_files)
  foreach(file ${MY_SCRIPTS})
    # Append "py" extension if needed
    get_filename_component(file_ext ${file} EXT)
    if(NOT "${file_ext}" MATCHES "py")
      set(file "${file}.py")
    endif()

    if(NOT IS_ABSOLUTE ${file})
      set(src "${MY_SOURCE_DIR}/${file}")
    else()
      set(src "${file}")
    endif()
    set(tgt_file ${file})
    if(IS_ABSOLUTE ${file})
      set(src ${file})
      file(RELATIVE_PATH tgt_file ${CMAKE_CURRENT_BINARY_DIR} ${file})
    endif()
    set_property(GLOBAL APPEND PROPERTY
      _CTK_${target}_PYTHON_SCRIPTS "${src}|${tgt_file}|${MY_DESTINATION_DIR}")
  endforeach()

  if(DEFINED MY_RESOURCES)
    set(resource_input_files)
    foreach(file ${MY_RESOURCES})
      if(NOT IS_ABSOLUTE ${file})
        set(src "${MY_SOURCE_DIR}/${file}")
      else()
        set(src "${file}")
      endif()
      set_property(GLOBAL APPEND PROPERTY
      _CTK_${target}_PYTHON_RESOURCES "${src}|${file}|${MY_DESTINATION_DIR}")
    endforeach()
  endif()

  set(MY_DIRECTORY_TO_INSTALL ${MY_DESTINATION_DIR})
  if(MY_NO_INSTALL_SUBDIR)
    set(MY_DIRECTORY_TO_INSTALL ${MY_DESTINATION_DIR}/)
  endif()

  # Install python module / resources directory
  install(DIRECTORY "${MY_DIRECTORY_TO_INSTALL}"
    DESTINATION "${MY_INSTALL_DIR}" COMPONENT RuntimeLibraries
    USE_SOURCE_PERMISSIONS)

  if(NOT MY_GLOBAL_TARGET)
    ctkFunctionAddCompilePythonScriptTargets(${target})
  endif()
endmacro()


function(_ctk_add_copy_python_files_target target type)
  # 'type' is expected to be either "Resource" or "Script"
  set(target_name Copy${target}Python${type}Files)
  if(NOT TARGET ${target_name})
    string(TOUPPER ${type} type_upper)
    get_property(entries GLOBAL PROPERTY _CTK_${target}_PYTHON_${type_upper}S)
    set(input_files)
    set(copied_files)
    foreach(entry IN LISTS entries)
      string(REPLACE "|" ";" tuple "${entry}")
      list(GET tuple 0 src)
      list(GET tuple 1 tgt_file)
      list(GET tuple 2 dest_dir)
      set(tgt ${dest_dir}/${tgt_file})
      add_custom_command(DEPENDS ${src}
                         COMMAND ${CMAKE_COMMAND} -E copy ${src} ${tgt}
                         OUTPUT ${tgt}
                         COMMENT "Copying python ${type}: ${tgt_file}")
      list(APPEND input_files ${src})
      list(APPEND copied_files ${tgt})
    endforeach()
    if(entries)
      add_custom_target(${target_name} ALL DEPENDS ${copied_files} ${ARGN})
    endif()
  endif()
endfunction()


function(_ctk_add_compile_python_directories_target target)
  set(target_name Compile${target}PythonFiles)
  if(NOT TARGET ${target_name})
    # Byte compile the Python files.
    set(compile_all_script "${CMAKE_CURRENT_BINARY_DIR}/compile_${target}_python_scripts.py")

    set(_compileall_code )
    get_property(entries GLOBAL PROPERTY _CTK_${target}_PYTHON_SCRIPTS)
    list(REMOVE_DUPLICATES entries)
    foreach(entry IN LISTS entries)
      string(REPLACE "|" ";" tuple "${entry}")
      list(GET tuple 1 tgt_file)
      list(GET tuple 2 dest_dir)
      set(tgt ${dest_dir}/${tgt_file})
      set(_compileall_code "${_compileall_code}\nctk_compile_file('${tgt}', force=1)")
    endforeach()

    if(NOT PYTHONINTERP_FOUND)
      find_package(PythonInterp REQUIRED)
    endif()
    if(NOT PYTHONLIBS_FOUND)
      find_package(PythonLibs REQUIRED)
    endif()

    # Extract python lib path
    ctkFunctionExtractOptimizedLibrary(PYTHON_LIBRARIES PYTHON_LIBRARY)
    get_filename_component(PYTHON_LIBRARY_PATH "${PYTHON_LIBRARY}" PATH)

    # Configure cmake script associated with the custom command
    # required to properly update the library path with PYTHON_LIBRARY_PATH
    set(compile_all_cmake_script "${CMAKE_CURRENT_BINARY_DIR}/compile_${target}_python_scripts.cmake")
    configure_file(
      ${CTK_CMAKE_DIR}/ctk_compile_python_scripts.cmake.in
      ${compile_all_cmake_script}
      @ONLY
      )

    add_custom_command(
      COMMAND ${CMAKE_COMMAND} -P ${compile_all_cmake_script}
      DEPENDS Copy${target}PythonScriptFiles ${compile_all_cmake_script}
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/python_compile_${target}_complete"
      COMMENT "Compiling python scripts: ${target}"
      )

    add_custom_target(${target_name} ALL
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/python_compile_${target}_complete
        )
  endif()
endfunction()

function(ctkFunctionAddCompilePythonScriptTargets target)
  _ctk_add_copy_python_files_target(${target} Script ${ARGN})
  _ctk_add_copy_python_files_target(${target} Resource ${ARGN})
  _ctk_add_compile_python_directories_target(${target})
endfunction()

