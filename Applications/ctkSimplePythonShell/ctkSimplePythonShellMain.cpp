
// Qt includes
#include <QApplication>

// CTK includes
#include <ctkPythonShell.h>

#include "ctkSimplePythonManager.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  
  ctkSimplePythonManager pythonManager;
  
  ctkPythonShell shell(&pythonManager);
  shell.setAttribute(Qt::WA_QuitOnClose, false);
  shell.resize(600, 280);
  shell.show();
  
  return app.exec();
}
