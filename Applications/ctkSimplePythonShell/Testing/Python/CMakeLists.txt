
set(KIT_TESTS ${CPP_TEST_PATH}/ctkSimplePythonShell)

set(SCRIPTS
  wrappedQInvokableTest.py
  wrappedQPropertyTest.py
  wrappedSlotTest.py
  )

# Since QTimer is part of QtCore and is used in both 'ctkWidgetsTest.py' and 'derivedQWidgetTest.py', 
# these two tests should be included only if CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QTCORE is ON

if(CTK_LIB_Widgets AND CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QTCORE)
  list(APPEND SCRIPTS ctkWidgetsTest.py)
endif()

if(CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QTCORE AND CTK_LIB_Scripting/Python/Core_PYTHONQT_WRAP_QTGUI)
  list(APPEND SCRIPTS derivedQWidgetTest.py)
endif()

if(CTK_LIB_Scripting/Python/Core_PYTHONQT_USE_VTK)
  list(APPEND SCRIPTS
    vtkPythonSmoke.py
    wrappedVTKQInvokableTest.py
    wrappedVTKSlotTest.py
    )
endif()


macro(SIMPLE_TEST_WITH_SCRIPT SCRIPT)
  get_filename_component(TESTNAME ${SCRIPT} NAME_WE)
  add_test(ctkSimplePythonShell_${TESTNAME} ${KIT_TESTS} ${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT})
endmacro()

foreach(s ${SCRIPTS})
  SIMPLE_TEST_WITH_SCRIPT(${s})
endforeach()
