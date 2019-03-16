from __future__ import print_function

from ctk import *
from qt import QTimer

# Test display of a CTK widget
w = ctkMatrixWidget()
w.show()

# Test command execution using Python manager
pythonManager = _ctkPythonManagerInstance
pythonManager.executeString("variableInPythonConsole=523")
try:
  print("variableInPythonConsole was successfully set to {0}".format(variableInPythonConsole))
except:
  print("PythonManager.executeString failed")
  qt.QApplication.exit(1)

if not _ctkPythonConsoleInstance.isInteractive:
  #QTimer().singleShot(0, app(), SLOT('quit()'))
  t = QTimer()
  t.setInterval(250)
  t.connect('timeout()', app(), 'quit()')
  t.start()
