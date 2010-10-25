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
#include <QTreeView>
#include <QSettings>
#include <QDir>

// CTK widget includes
#include <ctkDICOMQueryRetrieveWidget.h>

// ctkDICOMCore includes
#include "ctkDICOM.h"
#include "ctkDICOMModel.h"

// Logger
#include "ctkLogger.h"

// STD includes
#include <iostream>

int main(int argc, char** argv)
{
  ctkLogger::configure();
  QApplication app(argc, argv);

  app.setOrganizationName("commontk");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkDICOM");

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
    std::cerr << "No DatabaseDirectory on command line or in settings.  Using \"" << databaseDirectory.toLatin1().data() << "\".\n";
  } else
  {
    databaseDirectory = settings.value("DatabaseDirectory", "").toString();
  }

  QDir qdir(databaseDirectory);
  if ( !qdir.exists(databaseDirectory) ) 
  {
    if ( !qdir.mkpath(databaseDirectory) )
    {
      std::cerr << "Could not create database directory \"" << databaseDirectory.toLatin1().data() << "\".\n";
      return EXIT_FAILURE;
    }
  }


  QString databaseFileName = databaseDirectory + QString("/ctkDICOM.sql");

  ctkDICOM myCTK;
  try { myCTK.openDatabase( databaseFileName ); }
  catch (std::exception e)
  {
    std::cerr << "Database error:" << qPrintable(myCTK.GetLastError()) << "\n";
    myCTK.closeDatabase();
    return EXIT_FAILURE;
  }

  ctkDICOMModel model;
  model.setDatabase(myCTK.database());
  
  ctkDICOMQueryRetrieveWidget queryRetrieve;
  QTreeView *treeView = queryRetrieve.findChild<QTreeView *>("treeView");
  if (!treeView)
    {
    std::cerr << "Could not access tree view from QueryRetrieve widget\n";
    return EXIT_FAILURE;
    }
  treeView->setModel(&model);

  queryRetrieve.show();
  queryRetrieve.raise();
  return app.exec();
}
