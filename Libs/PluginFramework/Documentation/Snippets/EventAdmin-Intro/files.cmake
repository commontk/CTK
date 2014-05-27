set(snippet_src_files
  main.cpp
  ctkSnippetReportManager.h
)

set(_moc_files
  ctkSnippetReportManager.h
)

foreach(_moc_file ${_moc_files})
  if (CTK_QT_VERSION VERSION_GREATER "4")
    QT5_WRAP_CPP(snippet_src_files EventAdmin-Intro/${_moc_file}
      OPTIONS -f${CMAKE_CURRENT_SOURCE_DIR}/EventAdmin-Intro/${_moc_file})
  else()
    QT4_WRAP_CPP(snippet_src_files EventAdmin-Intro/${_moc_file}
      OPTIONS -f${CMAKE_CURRENT_SOURCE_DIR}/EventAdmin-Intro/${_moc_file})
  endif()
endforeach()
