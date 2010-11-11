#
# QtMobility
#
SET(QtMobility_DEPENDS)
ctkMacroShouldAddExternalProject(QTMOBILITY_QTSERVICEFW_LIBRARIES add_project)
IF(${add_project})
  SET(QtMobility_enabling_variable QTMOBILITY_QTSERVICEFW_LIBRARIES)
  SET(proj QtMobility)
#   MESSAGE(STATUS "Adding project:${proj}")
  SET(QtMobility_DEPENDS ${proj})

  # Patch program
  FIND_PROGRAM(CTK_PATCH_EXECUTABLE patch
    "C:/Program Files/GnuWin32/bin"
    "C:/Program Files (x86)/GnuWin32/bin")
  MARK_AS_ADVANCED(CTK_PATCH_EXECUTABLE)
  IF(NOT CTK_PATCH_EXECUTABLE)
    MESSAGE(FATAL_ERROR "error: Patch is required to build ${proj}. Set CTK_PATCH_EXECUTABLE. If running Windows, you can download it here: http://gnuwin32.sourceforge.net/packages/patch.htm")
  ENDIF()
  
  # Configure patch script
  SET(qtmobility_src_dir ${ep_source_dir}/${proj})
  SET(qtmobility_patch_dir ${CTK_SOURCE_DIR}/Utilities/QtMobility/)
  SET(qtmobility_configured_patch_dir ${CTK_BINARY_DIR}/Utilities/QtMobility/)
  SET(qtmobility_patchscript
    ${qtmobility_configured_patch_dir}/QtMobility-1.0.0-patch.cmake)
  CONFIGURE_FILE(
    ${qtmobility_patch_dir}/QtMobility-1.0.0-patch.cmake.in
    ${qtmobility_patchscript} @ONLY)

  # Define configure options
  SET(qtmobility_modules "serviceframework")
  SET(qtmobility_build_type "release")
  IF(UNIX OR MINGW)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      SET(qtmobility_build_type "debug")
    ENDIF()
  ELSEIF(NOT ${CMAKE_CFG_INTDIR} STREQUAL "Release")
    SET(qtmobility_build_type "debug")
  ENDIf()
  
  SET(qtmobility_make_cmd)
  SET(qtmobility_install_cmd)
  IF(UNIX)
    SET(qtmobility_make_cmd make)
    SET(qtmobility_config_args -${qtmobility_build_type} -libdir ${CTK_CMAKE_LIBRARY_OUTPUT_DIRECTORY} -no-docs -modules ${qtmobility_modules})
    SET(qtmobility_install_cmd ${qtmobility_make_cmd} install)
  ELSEIF(WIN32)
    SET(qtmobility_make_cmd nmake)
    IF(MINGW AND NOT MSYS)
      SET(qtmobility_make_cmd mingw32-make)
    ENDIF()

    SET(qtmobility_win32_install_prefix "${ep_source_dir}/${proj}/install/")
    STRING(REPLACE "/" "\\" qtmobility_win32_native_install_prefix ${qtmobility_win32_install_prefix})

    SET(qtmobility_config_args -${qtmobility_build_type} -qt ${QT_BINARY_DIR} -prefix ${qtmobility_win32_native_install_prefix} -no-docs -modules ${qtmobility_modules})

    CONFIGURE_FILE(${qtmobility_patch_dir}/QtMobility-1.0.0-install-win32.cmake.in
                   ${qtmobility_configured_patch_dir}/QtMobility-1.0.0-install-win32.cmake @ONLY)
				   
    SET(qtmobility_install_cmd ${CMAKE_COMMAND} -D INTERMEDIATE_DIRECTORY:STRING=$(IntDir) -P ${qtmobility_configured_patch_dir}/QtMobility-1.0.0-install-win32.cmake)
  ENDIF()
  
  ExternalProject_Add(${proj}
    URL ${CTK_SOURCE_DIR}/Utilities/QtMobility/qt-mobility-servicefw-opensource-src-1.0.0.tar.gz
    PATCH_COMMAND ${CMAKE_COMMAND} -P ${qtmobility_patchscript}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure ${qtmobility_config_args}
    BUILD_COMMAND ${qtmobility_make_cmd}
    INSTALL_COMMAND ${qtmobility_install_cmd}
    BUILD_IN_SOURCE 1
    )
    
  # Since the full path of QtMobility library is used, there is not need to add 
  # its corresponding library output directory to CTK_EXTERNAL_LIBRARY_DIRS

  IF(UNIX)
    SET(${QtMobility_enabling_variable}_INCLUDE_DIRS 
        "${CTK_BINARY_DIR}/CMakeExternals/Source/QtMobility/install/include")
  ELSE()
    SET(${QtMobility_enabling_variable}_INCLUDE_DIRS
        "${qtmobility_win32_install_prefix}/include")
  ENDIF()
	
ENDIF()
