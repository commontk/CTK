
from ctk import *
from qt import QTimer

w = ctkMatrixWidget()
w.show()

if not _ctkPythonConsoleInstance.isInteractive:
  #QTimer().singleShot(0, app(), SLOT('quit()'))
  t = QTimer()
  t.setInterval(250)
  t.connect('timeout()', app(), 'quit()')
  t.start()
