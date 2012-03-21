import qt

if (_testWrappedSlotInstance.value() != 0):
  qt.QApplication.exit(1)

_testWrappedSlotInstance.setValue(74)
if (_testWrappedSlotInstance.value() != 74):
  qt.QApplication.exit(1)

qt.QApplication.exit(0)
