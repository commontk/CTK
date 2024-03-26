/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTimer>
#include <QPushButton>

// CTK includes
#include "ctkPythonConsole.h"
#include "ctkAbstractPythonManager.h"
#include "ctkCompleter.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkPythonConsoleTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QPushButton button("Show PythonConsole");

  ctkPythonConsole pythonConsole;
  ctkAbstractPythonManager pythonManager;
  pythonConsole.initialize(&pythonManager);

  QList<QKeySequence> otherShortcuts;
  otherShortcuts << QKeySequence(Qt::CTRL + Qt::Key_Space);
  otherShortcuts << Qt::Key_F1;
  pythonConsole.setCompleterShortcuts(otherShortcuts);
  pythonConsole.addCompleterShortcut(Qt::Key_Tab);

  QObject::connect(&button, SIGNAL(clicked()), &pythonConsole, SLOT(show()));

  button.show();

  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(100);
  }

  return app.exec();
}
