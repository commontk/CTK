/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

// CTK widget includes
#include <ctkDICOMQueryRetrieveWidget.h>

// ctkDICOMCore includes
#include "ctkDICOM.h"
#include "ctkDICOMModel.h"

// STD includes
#include <iostream>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // set up the database 
  const char *datbaseFileName = "/tmp/test.db";
  const char *datbaseScriptFileName = "/Users/pieper/ctk/latest/CTK/Libs/DICOM/Core/Resources/dicom-sample.sql";
  if (argc > 1)
    {
    datbaseFileName = argv[1];
    }
  if (argc > 2)
    {
    datbaseScriptFileName = argv[2];
    }

  ctkDICOM myCTK;
  try { myCTK.openDatabase( datbaseFileName ); }
  catch (std::exception e)
  {
    std::cerr << "Database error:" << qPrintable(myCTK.GetLastError());
    myCTK.closeDatabase();
    return EXIT_FAILURE;
  }

  try { myCTK.initializeDatabase(datbaseScriptFileName); }
  catch (std::exception e)
  {
    std::cerr << "Error when initializing the data base: " << datbaseScriptFileName
              << " error: " << myCTK.GetLastError().toStdString();
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
  return app.exec();
}
