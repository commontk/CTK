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
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMPatientFilterProxyModel.h"

// STD includes
#include <iostream>

int ctkDICOMPatientFilterProxyModelTest1(int argc, char* argv[])
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

    // Create patient model
    ctkDICOMPatientModel sourceModel;
    sourceModel.setDicomDatabase(database);

    // Create filter proxy model
    ctkDICOMPatientFilterProxyModel proxyModel;
    proxyModel.setSourceModel(&sourceModel);

    // Test default values
    CHECK_INT(proxyModel.displayMode(), ctkDICOMPatientFilterProxyModel::TabMode);

    // Test display mode
    proxyModel.setDisplayMode(ctkDICOMPatientFilterProxyModel::ListMode);
    CHECK_INT(proxyModel.displayMode(), ctkDICOMPatientFilterProxyModel::ListMode);

    proxyModel.setDisplayMode(ctkDICOMPatientFilterProxyModel::TabMode);
    CHECK_INT(proxyModel.displayMode(), ctkDICOMPatientFilterProxyModel::TabMode);

    // Test widget width
    proxyModel.setWidgetWidth(800);
    CHECK_INT(proxyModel.widgetWidth(), 800);

    // Test spacing
    proxyModel.setSpacing(10);
    CHECK_INT(proxyModel.spacing(), 10);

    // Test icon size
    proxyModel.setIconSize(32);
    CHECK_INT(proxyModel.iconSize(), 32);

    // Test max text width
    proxyModel.setMaxTextWidth(200);
    CHECK_INT(proxyModel.maxTextWidth(), 200);

    // Refresh source model
    sourceModel.refresh();

    // Test filtering (filters by IsVisibleRole)
    int sourceRows = sourceModel.rowCount();
    int proxyRows = proxyModel.rowCount();

    // Proxy should filter based on visibility
    CHECK_BOOL(proxyRows <= sourceRows, true);

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
