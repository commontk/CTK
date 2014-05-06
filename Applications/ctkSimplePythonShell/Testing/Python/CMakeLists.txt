
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
    wrappedVTKObserverTest.py
    wrappedVTKQInvokableTest.py
    wrappedVTKSlotTest.py
    )
endif()

foreach(script ${SCRIPTS})
  get_filename_component(scriptname ${script} NAME_WE)
  add_test(
    NAME ctkSimplePythonShell_${scriptname}
    COMMAND $<TARGET_FILE:ctkSimplePythonShell> ${CMAKE_CURRENT_SOURCE_DIR}/${script}
    )
  set_property(TEST ctkSimplePythonShell_${scriptname} PROPERTY LABELS ${KIT})
endforeach()

