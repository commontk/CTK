
# QT4_GENERATE_MOCS(inputfile1 [inputfile2 ...])

macro(QT4_GENERATE_MOCS)
  foreach(file ${ARGN})
    set(moc_file moc_${file})
    if(CTK_USE_QT5)
      qt5_generate_moc(${file} ${moc_file})
    else()
      QT4_GENERATE_MOC(${file} ${moc_file})
    endif()

    get_filename_component(source_name ${file} NAME_WE)
    get_filename_component(source_ext ${file} EXT)
    if(${source_ext} MATCHES "\\.[hH]")
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.cpp)
        set(source_ext .cpp)
      elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.cxx)
        set(source_ext .cxx)
      endif()
    endif()
    set_property(SOURCE ${source_name}${source_ext} APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${moc_file})
  endforeach()
endmacro()

# create a Qt5 alias
macro(QT5_GENERATE_MOCS)
  QT4_GENERATE_MOCS(${ARGN})
endmacro()
