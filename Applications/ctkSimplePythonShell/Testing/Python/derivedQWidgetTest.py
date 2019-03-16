from __future__ import division

from qt import *

class ExampleWidget(QWidget):
  def __init__(self, parent=None):
    QWidget.__init__(self, parent)

    self.setGeometry(300, 300, 250, 150)
    self.setWindowTitle('Example Widget')
    
    layout = QHBoxLayout(self)
    
    checkbox = QCheckBox('Shell Visibility', self)
    layout.addWidget(checkbox);
    checkbox.setChecked(True)
    checkbox.connect('toggled(bool)', _ctkPythonConsoleInstance, 'setVisible(bool)')
    
    self.button = QPushButton('Quit', self)
    layout.addWidget(self.button);
    QObject.connect(self.button, SIGNAL('clicked()'), app(), SLOT('quit()'))
    
    self.center()

  def center(self):
    screen = QDesktopWidget().screenGeometry()
    size =  self.geometry
    self.move(int((screen.width() - size.width())/2), int((screen.height() - size.height())/2))
    
w = ExampleWidget()
w.show()

if not _ctkPythonConsoleInstance.isInteractive:
  #QTimer().singleShot(250, app(), SLOT('quit()'))
  t = QTimer()
  t.setInterval(250)
  t.connect('timeout()', app(), 'quit()')
  t.start()
  
  


