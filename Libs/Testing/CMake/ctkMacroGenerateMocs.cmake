
# QT4_GENERATE_MOCS(inputfiles ... )

macro (QT4_GENERATE_MOCS)

  foreach (fileIt ${ARGN})
    QT4_GENERATE_MOC( ${fileIt} moc_${fileIt})
    macro_add_file_dependencies(${fileIt} moc_${fileIt})
  endforeach()

endmacro()
