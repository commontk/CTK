
# QT4_GENERATE_MOCS(inputfile1 [inputfile2 ...])

macro(QT4_GENERATE_MOCS)
  QT4_GET_MOC_FLAGS(_moc_flags)

  foreach(file ${ARGN})
    set(moc_file moc_${file})

    get_filename_component(source_name ${file} NAME_WE)
    get_filename_component(source_ext ${file} EXT)
    if(${source_ext} MATCHES "\\.[hH]")
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.cpp)
        set(source_ext .cpp)
      elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.cxx)
        set(source_ext .cxx)
      endif()
    endif()

    set(_cpp ${source_name}${source_ext})
    set(_moc ${CMAKE_CURRENT_BINARY_DIR}/${moc_file})

    QT4_CREATE_MOC_COMMAND(${_cpp} ${_moc} "${_moc_flags}" "")
    MACRO_ADD_FILE_DEPENDENCIES(${_cpp} ${_moc})

  endforeach()
endmacro()

