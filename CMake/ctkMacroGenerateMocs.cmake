
# QT4_GENERATE_MOCS(inputfile1 [inputfile2 ...])

include(MacroAddFileDependencies)

function(QT4_GENERATE_MOCS)
  if(CTK_QT_VERSION VERSION_GREATER "4")
    QT5_GET_MOC_FLAGS(_moc_flags)
  else()
    QT4_GET_MOC_FLAGS(_moc_flags)
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

    if(CTK_QT_VERSION VERSION_GREATER "4")
      if(Qt5_VERSION VERSION_LESS "5.6")
        QT5_CREATE_MOC_COMMAND(${abs_file} ${moc_file} "${_moc_flags}" "" "")
      else()
        QT5_CREATE_MOC_COMMAND(${abs_file} ${moc_file} "${_moc_flags}" "" "" "")
      endif()
    else()
      QT4_CREATE_MOC_COMMAND(${abs_file} ${moc_file} "${_moc_flags}" "" "")
    endif()
    MACRO_ADD_FILE_DEPENDENCIES(${abs_file} ${moc_file})
  endforeach()
endfunction()

# create a Qt5 alias
macro(QT5_GENERATE_MOCS)
  QT4_GENERATE_MOCS(${ARGN})
endmacro()
