#
# KWStyle
#
SET(kwstyle_DEPENDS)
IF(CTK_USE_KWSTYLE)
  
  # Sanity checks
  IF(DEFINED CTK_KWSTYLE_EXECUTABLE AND NOT EXISTS ${CTK_KWSTYLE_EXECUTABLE})
    MESSAGE(FATAL_ERROR "CTK_KWSTYLE_EXECUTABLE variable is defined but corresponds to non-existing executable")
  ENDIF()
  
  SET(proj KWStyle-CVSHEAD)
  SET(proj_DEPENDENCIES)
  
  SET(kwstyle_DEPENDS ${proj})
  
  IF(NOT DEFINED CTK_KWSTYLE_EXECUTABLE)
    ExternalProject_Add(${proj}
      LIST_SEPARATOR ${sep}
      CVS_REPOSITORY ":pserver:anoncvs:@public.kitware.com:/cvsroot/KWStyle"
      CVS_MODULE "KWStyle"
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${ep_common_args}
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(CTK_KWSTYLE_EXECUTABLE ${ep_install_dir}/bin/KWStyle)
    
    # Since KWStyle is an executable, there is not need to add its corresponding 
    # library output directory to CTK_EXTERNAL_LIBRARY_DIRS
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
ENDIF()
