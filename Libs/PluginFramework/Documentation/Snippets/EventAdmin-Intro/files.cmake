set(snippet_src_files
  main.cpp
  ctkSnippetReportManager.h
)

set(_moc_files
  ctkSnippetReportManager.h
)

foreach(_moc_file ${_moc_files})
  if(CTK_QT_VERSION VERSION_EQUAL "5")
    QT5_WRAP_CPP(snippet_src_files EventAdmin-Intro/${_moc_file}
      OPTIONS -f${CMAKE_CURRENT_SOURCE_DIR}/EventAdmin-Intro/${_moc_file})
  else()
    message(FATAL_ERROR "Support for Qt${CTK_QT_VERSION} is not implemented")
  endif()
endforeach()
