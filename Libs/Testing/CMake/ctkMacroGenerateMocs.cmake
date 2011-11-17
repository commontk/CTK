
# QT4_GENERATE_MOCS(inputfiles ... )

MACRO (QT4_GENERATE_MOCS)

  FOREACH (fileIt ${ARGN})
    QT4_GENERATE_MOC( ${fileIt} moc_${fileIt})
    MACRO_ADD_FILE_DEPENDENCIES(${fileIt} moc_${fileIt})
  ENDFOREACH(fileIt)

ENDMACRO (QT4_GENERATE_MOCS)
