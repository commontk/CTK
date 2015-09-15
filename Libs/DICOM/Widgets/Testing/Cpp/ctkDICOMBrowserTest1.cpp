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

// ctk includes
#include "ctkUtils.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// ctkDICOMWidget includes
#include "ctkDICOMBrowser.h"

// STD includes
#include <iostream>

int ctkDICOMBrowserTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  qDebug() << "argc = " << argc;
  for (int i = 0; i < argc; ++i)
    {
    qDebug() << "\t" << argv[i];
    }

  QFileInfo tempFileInfo(QDir::tempPath() + QString("/ctkDICOMBrowserTest1-db"));
  QString dbDir = tempFileInfo.absoluteFilePath();
  qDebug() << "\n\nUsing directory: " << dbDir;
  if (tempFileInfo.exists())
    {
    qDebug() << "\n\nRemoving directory: " << dbDir;
    ctk::removeDirRecursively(dbDir);
    }
  qDebug() << "\n\nMaking directory: " << dbDir;
  QDir dir(dbDir);
  dir.mkdir(dbDir);

  ctkDICOMBrowser browser;
  browser.setDatabaseDirectory(dbDir);

  browser.show();

  browser.setDisplayImportSummary(false);
  qDebug() << "Importing directory " << argv[1];

  // make sure copy/link dialog doesn't pop up, always copy on import
  QSettings settings;
  QString settingsString = settings.value("MainWindow/DontConfirmCopyOnImport").toString();
  settings.setValue("MainWindow/DontConfirmCopyOnImport", QString("0"));

  browser.onImportDirectory(argv[1]);

  // reset to the original copy/import setting
  settings.setValue("MainWindow/DontConfirmCopyOnImport", settingsString);

  if (browser.patientsAddedDuringImport() != 1
    || browser.studiesAddedDuringImport() != 1
    || browser.seriesAddedDuringImport() != 1
    || browser.instancesAddedDuringImport() != 100)
    {
    qDebug() << "\n\nDirectory did not import as expected!\n\n";
    return EXIT_FAILURE;
    }

  qDebug() << "\n\nAdded to database directory: " << dbDir;

  if (argc <= 2 || QString(argv[argc - 1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }



  return app.exec();
}
