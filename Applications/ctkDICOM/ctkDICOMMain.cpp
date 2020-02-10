/*=========================================================================

  Library:   CTK

  Copyright (c) Isomics Inc.

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
#include <QTreeView>
#include <QSettings>
#include <QDir>
#include <QResource>

// CTK widget includes
#include <ctkDICOMAppWidget.h>

// ctkDICOMCore includes
#include "ctkDICOMBrowser.h"
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"

// Logger
#include "ctkLogger.h"

// STD includes
#include <iostream>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  app.setOrganizationName("commontk");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkDICOM");

  // set up Qt resource files
  QResource::registerResource("./Resources/ctkDICOM.qrc");

  QSettings settings;
  QString databaseDirectory;

  // set up the database
  if (argc > 1)
  {
    QString directory(argv[1]);
    settings.setValue("DatabaseDirectory", directory);
    settings.sync();
  }

  if ( settings.value("DatabaseDirectory", "") == "" )
  {
    databaseDirectory = QString("./ctkDICOM-Database");
    std::cerr << "No DatabaseDirectory on command line or in settings.  Using \"" << qPrintable(databaseDirectory) << "\".\n";
  }
  else
  {
    databaseDirectory = settings.value("DatabaseDirectory", "").toString();
  }

  QDir qdir(databaseDirectory);
  if ( !qdir.exists(databaseDirectory) )
  {
    if ( !qdir.mkpath(databaseDirectory) )
    {
      std::cerr << "Could not create database directory \"" << qPrintable(databaseDirectory) << "\".\n";
      return EXIT_FAILURE;
    }
  }

  ctkDICOMAppWidget DICOMApp;

  DICOMApp.setDatabaseDirectory(databaseDirectory);
  DICOMApp.show();

  return app.exec();
}
