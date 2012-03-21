import qt

if (_testWrappedQInvokableInstance.value() != 0):
  qt.QApplication.exit(1)

_testWrappedQInvokableInstance.setValue(74)
if (_testWrappedQInvokableInstance.value() != 74):
  qt.QApplication.exit(1)

qt.QApplication.exit(0)
