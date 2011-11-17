#!
#! \brief Compile a CMake doxygen input filter
#!
#! \param OUT &lt;out-file&gt; (optional) Supply an absolute filename for
#!                             the generated executable.
#! \param NAMESPACE &lt;namespace&gt; (optional) Supply a C++ namespace in
#!                                    which the generated function declrarations
      #!                              should be wrapped.
#!
#! \return This function sets the <code>CMakeDoxygenFilter_EXECUTABLE</code>
#!         variable to the absolute path of the generated input filter executable
#!         in the parent scope. If <out-file> is specified, they will be the same.
#!
#! This CMake function compiles the http://github.com/saschazelzer/CMakeDoxygenFilter
#! project into a doxygen input filter executable. See
#! http://github.com/saschazelzer/CMakeDoxygenFilter/blob/master/README for more details.
#!
#! \ingroup CMakeUtilities
function(ctkFunctionCMakeDoxygenFilterCompile)

  #-------------------- parse function arguments -------------------

  set(DEFAULT_ARGS)
  set(prefix "FILTER")
  set(arg_names "OUT;NAMESPACE")
  set(option_names "")

  foreach(arg_name ${arg_names})
    set(${prefix}_${arg_name})
  endforeach()

  foreach(option ${option_names})
    set(${prefix}_${option} FALSE)
  endforeach()

  set(current_arg_name DEFAULT_ARGS)
  set(current_arg_list)

  foreach(arg ${ARGN})
    set(larg_names ${arg_names})
    list(FIND larg_names "${arg}" is_arg_name)
    if(is_arg_name GREATER -1)
      set(${prefix}_${current_arg_name} ${current_arg_list})
      set(current_arg_name "${arg}")
      set(current_arg_list)
    else()
      set(loption_names ${option_names})
      list(FIND loption_names "${arg}" is_option)
      if(is_option GREATER -1)
        set(${prefix}_${arg} TRUE)
      else()
        set(current_arg_list ${current_arg_list} "${arg}")
      endif()
    endif()
  endforeach()

  set(${prefix}_${current_arg_name} ${current_arg_list})

  #------------------- finished parsing arguments ----------------------

  if(FILTER_OUT)
    set(copy_file "${FILTER_OUT}")
  else()
    set(copy_file "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CMakeDoxygenFilter${CMAKE_EXECUTABLE_SUFFIX}")
  endif()

  set(compile_defs "")
  if(FILTER_NAMESPACE)
    set(compile_defs "${compile_defs} -DUSE_NAMESPACE=${FILTER_NAMESPACE}")
  endif()

  set(cmake_doxygen_filter_src "${CTK_SOURCE_DIR}/Documentation/CMakeDoxygenFilter.cpp")

  try_compile(result_var
              "${CMAKE_CURRENT_BINARY_DIR}"
              "${cmake_doxygen_filter_src}"
              COMPILE_DEFINITIONS ${compile_defs}
              OUTPUT_VARIABLE compile_output
              COPY_FILE ${copy_file}
             )

  if(NOT result_var)
    message(FATAL_ERROR "error: Faild to compile ${cmake_doxygen_filter_src} (result: ${result_var})\n${compile_output}")
  endif()

  set(CMakeDoxygenFilter_EXECUTABLE "${copy_file}" PARENT_SCOPE)

endfunction()
