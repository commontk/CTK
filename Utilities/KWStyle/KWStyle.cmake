if( CTK_USE_KWSTYLE )
  set(msvc_default OFF)
  if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(msvc_default ON)
  endif()

  option(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
  option(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." ${msvc_default})
  option(KWSTYLE_USE_GCC_FORMAT "Set KWStyle to generate errors with a GCC-compatible format." OFF)

  find_program(KWSTYLE_EXECUTABLE
    NAMES KWStyle
    PATHS
    /usr/local/bin
    )

  configure_file(
    ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/qCTKFiles.txt.in
    ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTKFiles.txt)

  configure_file(
    ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/qCTK.kws.xml.in
    ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTK.kws.xml)


  set(CTK_KWSTYLE_ARGUMENTS
    -xml ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTK.kws.xml -v
      -D ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTKFiles.txt
      -o ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/qCTKOverwrite.txt
    )

  if(KWSTYLE_USE_VIM_FORMAT)
    list(APPEND CTK_KWSTYLE_ARGUMENTS -vim)
  endif()

  if(KWSTYLE_USE_MSVC_FORMAT)
    list(APPEND CTK_KWSTYLE_ARGUMENTS -msvc)
  endif()

  if(KWSTYLE_USE_GCC_FORMAT)
    list(APPEND CTK_KWSTYLE_ARGUMENTS -gcc)
  endif()

  add_custom_command(
    OUTPUT ${CTK_BINARY_DIR}/qCTKStyleReport.txt
    COMMAND ${KWSTYLE_EXECUTABLE}
    ARGS    ${CTK_KWSTYLE_ARGUMENTS}
    COMMENT "Coding Style Checker qCTK style"
    )

  add_custom_target(qCTKStyleCheck DEPENDS ${CTK_BINARY_DIR}/qCTKStyleReport.txt)
  add_test(qCTKStyleTest ${KWSTYLE_EXECUTABLE} ${CTK_KWSTYLE_ARGUMENTS})

endif()
