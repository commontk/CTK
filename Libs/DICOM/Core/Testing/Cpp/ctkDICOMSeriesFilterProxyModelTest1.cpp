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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesFilterProxyModel.h"

// STD includes
#include <iostream>

int ctkDICOMSeriesFilterProxyModelTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  bool interactive = arguments.removeOne("-I");

  if (arguments.count() != 2)
  {
    std::cerr << "Usage: " << qPrintable(testName)
              << " [-I] <scratch.db> <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
  }

  QString databaseFile(arguments.at(0));
  QString sqlFileName(arguments.at(1));

  try
  {
    // Initialize database
    ctkDICOMDatabase database(databaseFile);
    if (!database.initializeDatabase(sqlFileName.toUtf8()))
    {
      std::cerr << "Error initializing database: " << qPrintable(database.lastError()) << std::endl;
      return EXIT_FAILURE;
    }

    // Create series model
    ctkDICOMSeriesModel sourceModel;
    sourceModel.setDicomDatabase(database);

    // Create filter proxy model
    ctkDICOMSeriesFilterProxyModel proxyModel;
    proxyModel.setSourceModel(&sourceModel);

    // Test default values
    CHECK_INT(proxyModel.gridColumns(), 1000);

    // Test setting grid columns
    proxyModel.setGridColumns(3);
    CHECK_INT(proxyModel.gridColumns(), 3);

    proxyModel.setGridColumns(5);
    CHECK_INT(proxyModel.gridColumns(), 5);

    // Test that row count and column count reflect grid layout
    sourceModel.refresh();
    int sourceRows = sourceModel.rowCount();

    if (sourceRows > 0)
    {
      int expectedRows = (sourceRows + proxyModel.gridColumns() - 1) / proxyModel.gridColumns();
      CHECK_INT(proxyModel.rowCount(), expectedRows);
      CHECK_INT(proxyModel.columnCount(), proxyModel.gridColumns());
    }

    // Test sorting (series are sorted by series number)
    proxyModel.sort(0);

    if (!interactive)
    {
      QTimer::singleShot(200, &app, SLOT(quit()));
    }

    return app.exec();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
