
macro(ctkQtTesting SRCS_OUTPUT_VAR MOC_CPP_OUTPUT_VAR UI_FORMS_OUTPUT_VAR RESOURCES_OUTPUT_VAR)
  # Do not export symbol in ctkEventTranslatorPlayerWidget class
  add_definitions(-DNO_SYMBOL_EXPORT)

  include_directories(
    ${CTK_SOURCE_DIR}/Libs/QtTesting
    ${CTK_BINARY_DIR}/Libs/QtTesting
    )

  list(APPEND ${SRCS_OUTPUT_VAR}
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkEventTranslatorPlayerWidget.h
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkEventTranslatorPlayerWidget.cpp
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkXMLEventSource.h
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkXMLEventSource.cpp
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkXMLEventObserver.h
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkXMLEventObserver.cpp
    )
  list(APPEND ${MOC_CPP_OUTPUT_VAR}
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkEventTranslatorPlayerWidget.h
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkXMLEventSource.h
    ${CTK_SOURCE_DIR}/Libs/QtTesting/ctkXMLEventObserver.h
    )
  list(APPEND ${UI_FORMS_OUTPUT_VAR}
    ${CTK_SOURCE_DIR}/Libs/QtTesting/Resources/UI/ctkEventTranslatorPlayerWidget.ui
    )
  list(APPEND ${RESOURCES_OUTPUT_VAR}
    ${CTK_SOURCE_DIR}/Libs/QtTesting/Resources/ctkQtTesting.qrc
    )
endmacro()
