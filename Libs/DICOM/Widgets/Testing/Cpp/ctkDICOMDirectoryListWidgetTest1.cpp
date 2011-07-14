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
#include <QDir>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMDirectoryListWidget.h"

// STD includes
#include <iostream>

int ctkDICOMDirectoryListWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDICOMDatabase database;
  ctkDICOMDirectoryListWidget listWidget;
  listWidget.setDICOMDatabase(0);
  listWidget.setDICOMDatabase(&database);
  listWidget.addDirectory(QDir::tempPath());
  listWidget.removeDirectory();

  listWidget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  // if Qt uses the native dialog, then there is no way to force
  // the getExistingDirectory dialog to close. so we can't test it.
  //listWidget.addDirectory();

  return app.exec();
}
