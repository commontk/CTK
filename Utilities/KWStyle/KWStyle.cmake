IF( CTK_USE_KWSTYLE )
  SET(msvc_default OFF)
  IF(CMAKE_GENERATOR MATCHES "Visual Studio")
    SET(msvc_default ON)
  ENDIF()

  OPTION(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
  OPTION(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." ${msvc_default})
  OPTION(KWSTYLE_USE_GCC_FORMAT "Set KWStyle to generate errors with a GCC-compatible format." OFF)

  FIND_PROGRAM(CTK_KWSTYLE_EXECUTABLE
    NAMES KWStyle
    PATHS
    /usr/local/bin
    )

  CONFIGURE_FILE(
    ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/qCTKFiles.txt.in
    ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTKFiles.txt)

  CONFIGURE_FILE(
    ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/qCTK.kws.xml.in
    ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTK.kws.xml)


  SET(CTK_KWSTYLE_ARGUMENTS
    -xml ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTK.kws.xml -v
      -D ${PROJECT_BINARY_DIR}/Utilities/KWStyle/qCTKFiles.txt  
      -o ${PROJECT_SOURCE_DIR}/Utilities/KWStyle/qCTKOverwrite.txt
    )

  IF(KWSTYLE_USE_VIM_FORMAT)
    LIST(APPEND CTK_KWSTYLE_ARGUMENTS -vim)
  ENDIF()

  IF(KWSTYLE_USE_MSVC_FORMAT)
    LIST(APPEND CTK_KWSTYLE_ARGUMENTS -msvc)
  ENDIF()

  IF(KWSTYLE_USE_GCC_FORMAT)
    LIST(APPEND CTK_KWSTYLE_ARGUMENTS -gcc)
  ENDIF()

  ADD_CUSTOM_COMMAND(
    OUTPUT ${CTK_BINARY_DIR}/qCTKStyleReport.txt
    COMMAND ${CTK_KWSTYLE_EXECUTABLE}
    ARGS    ${CTK_KWSTYLE_ARGUMENTS}
    COMMENT "Coding Style Checker qCTK style"
    )

  ADD_CUSTOM_TARGET(qCTKStyleCheck DEPENDS ${CTK_BINARY_DIR}/qCTKStyleReport.txt)
  ADD_TEST(qCTKStyleTest ${CTK_KWSTYLE_EXECUTABLE} ${CTK_KWSTYLE_ARGUMENTS})

ENDIF( CTK_USE_KWSTYLE )
