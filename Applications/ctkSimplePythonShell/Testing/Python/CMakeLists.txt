
SET(KIT_TESTS ${CPP_TEST_PATH}/ctkSimplePythonShell)

SET(SCRIPTS
  derivedQWidgetTest.py
  wrappedQInvokableTest.py
  wrappedQPropertyTest.py
  wrappedSlotTest.py
  )

IF(CTK_LIB_Widgets)
  LIST(APPEND SCRIPTS ctkWidgetsTest.py)
ENDIF()

IF(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  LIST(APPEND SCRIPTS
    vtkPythonSmoke.py
    wrappedVTKQInvokableTest.py
    wrappedVTKSlotTest.py
    )
ENDIF()


MACRO(SIMPLE_TEST_WITH_SCRIPT SCRIPT)
  GET_FILENAME_COMPONENT(TESTNAME ${SCRIPT} NAME_WE)
  ADD_TEST(ctkSimplePythonShell_${TESTNAME} ${KIT_TESTS} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT})
ENDMACRO()

FOREACH(s ${SCRIPTS})
  SIMPLE_TEST_WITH_SCRIPT(${s})
ENDFOREACH()
