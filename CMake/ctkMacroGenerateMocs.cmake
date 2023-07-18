
# QT5_GENERATE_MOCS(inputfile1 [inputfile2 ...])

include(MacroAddFileDependencies)

function(_ctk_generate_mocs)
  if(CTK_QT_VERSION VERSION_EQUAL "5")
    if(Qt5_VERSION VERSION_LESS "5.15.0")
      QT5_GET_MOC_FLAGS(_moc_flags)
    else()
       # _moc_flags is not needed because it is internally handled
       # by qt5_generate_moc called below.
    endif()
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
  endif()
  foreach(file ${ARGN})

    get_filename_component(abs_file ${file} ABSOLUTE)

    get_filename_component(source_name ${file} NAME_WE)
    get_filename_component(source_ext ${file} EXT)
    if(${source_ext} MATCHES "\\.[hH]")
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.cpp)
        set(source_ext .cpp)
      elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.cxx)
        set(source_ext .cxx)
      endif()
    endif()

    set(moc_file ${CMAKE_CURRENT_BINARY_DIR}/moc_${source_name}${source_ext})

    if(CTK_QT_VERSION VERSION_EQUAL "5")
      if(Qt5_VERSION VERSION_LESS "5.6")
        QT5_CREATE_MOC_COMMAND(${abs_file} ${moc_file} "${_moc_flags}" "" "")
      elseif(Qt5_VERSION VERSION_LESS "5.15.0")
        QT5_CREATE_MOC_COMMAND(${abs_file} ${moc_file} "${_moc_flags}" "" "" "")
      else()
        # qt5_generate_moc internally calls qt5_get_moc_flags and ensure
        # no warnings are reported.
        qt5_generate_moc(${abs_file} ${moc_file})
      endif()
    else()
      message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
    endif()
    MACRO_ADD_FILE_DEPENDENCIES(${abs_file} ${moc_file})
  endforeach()
endfunction()

# create a Qt5 alias
macro(QT5_GENERATE_MOCS)
  _ctk_generate_mocs(${ARGN})
endmacro()
