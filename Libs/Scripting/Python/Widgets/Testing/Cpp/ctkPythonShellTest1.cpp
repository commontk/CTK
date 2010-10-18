/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

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
#include "ctkPythonShell.h"
#include "ctkAbstractPythonManager.h"

// STD includes
#include <stdlib.h>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkPythonShellTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QPushButton button("Show PythonShell");

  ctkPythonShell pythonShell(new ctkAbstractPythonManager);

  QObject::connect(&button, SIGNAL(clicked()), &pythonShell, SLOT(show()));

  button.show();

  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(100);
    }

  return app.exec();
}

