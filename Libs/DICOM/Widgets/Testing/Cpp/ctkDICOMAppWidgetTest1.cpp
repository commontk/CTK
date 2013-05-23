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
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QString>

// ctk includes
#include "ctkUtils.h"

// ctkDICOMCore includes
#include "ctkDICOMAppWidget.h"

// STD includes
#include <iostream>

/* Test from build directory:
 ./CTK-build/bin/CTKDICOMWidgetsCppTests ctkDICOMAppWidgetTest1 <test directory>

 if the test directory does not have 100 instances in one patient/study/series, test will fail
*/

int ctkDICOMAppWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  ctkDICOMAppWidget appWidget;

  QFileInfo tempFileInfo(QDir::tempPath() + QString("/ctkDICOMAppWidgetTest1-db"));
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

  appWidget.setDatabaseDirectory(dbDir);
  QString testString = QString("Test String");
  appWidget.onFileIndexed(testString);
  appWidget.openImportDialog();
  appWidget.openExportDialog();
  appWidget.openQueryDialog();

  appWidget.openQueryDialog();
  
  appWidget.setDisplayImportSummary(false);
  appWidget.onImportDirectory(argv[argc -1]);
  if ( appWidget.patientsAddedDuringImport() != 1
    || appWidget.studiesAddedDuringImport() != 1
    || appWidget.seriesAddedDuringImport() != 1
    || appWidget.instancesAddedDuringImport() != 100)
    {
    qDebug() << "\n\nDirectory did not import as expected!\n\n";
    return EXIT_FAILURE;
    }

  if (argc <= 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  qDebug() << "\n\nAdded to database directory: " << dbDir;
  return app.exec();
}
