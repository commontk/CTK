
import qt

if (_testWrappedQPropertyInstance.value != 0):
  qt.QApplication.exit(1)

_testWrappedQPropertyInstance.value = 74
if (_testWrappedQPropertyInstance.value != 74):
  qt.QApplication.exit(1)

qt.QApplication.exit(0)
