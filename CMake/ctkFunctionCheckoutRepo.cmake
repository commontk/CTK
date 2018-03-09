function(ctkFunctionCheckoutRepo)

  ctkMacroParseArguments("" "NAME;GIT_URL;GIT_TAG;GIT_PROTOCOL;CHECKOUT_DIR" "" ${ARGN})

  if(${_NAME}_DIR)
    # if a *_DIR variable is supplied, the repository is not cloned.
    return()
  endif()

  foreach(_required_arg NAME GIT_URL)
    if(NOT _${_required_arg})
      message(FATAL_ERROR "${_required_arg} is empty")
    endif()
  endforeach()
  
  if(NOT _GIT_PROTOCOL)
    set(_GIT_PROTOCOL ${EP_GIT_PROTOCOL})
  endif()
  if(NOT _CHECKOUT_DIR)
    set(_CHECKOUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  endif()
  if(NOT IS_ABSOLUTE "${_CHECKOUT_DIR}")
    message(FATAL_ERROR "The value of the CHECKOUT_DIR argument must be an absolute path.")
  endif()
  
  if(NOT _GIT_TAG)
    set(_GIT_TAG origin/master)
  endif()
  
  set(_repo_build_dir ${CMAKE_CURRENT_BINARY_DIR}/${_NAME}-proj)
  configure_file(${CTK_CMAKE_DIR}/ctkCheckoutRepo.cmake.in
                 ${_repo_build_dir}/CMakeLists.txt)

  execute_process(
    COMMAND ${CMAKE_COMMAND} .
    WORKING_DIRECTORY ${_repo_build_dir}
    RESULT_VARIABLE _result_code
    ERROR_VARIABLE _err_msg
    )
  if(_result_code)
    message(FATAL_ERROR "Configuring directory ${_repo_build_dir} failed: ${_err_msg}")
  endif()
  
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${_repo_build_dir}
    RESULT_VARIABLE _result_code
    ERROR_VARIABLE _err_msg
    )
  if(_result_code)
    message(FATAL_ERROR "Building directory ${_repo_build_dir} failed: ${_err_msg}")
  endif()
  
  set(${_NAME}_DIR ${_CHECKOUT_DIR}/${_NAME} PARENT_SCOPE)

endfunction()
