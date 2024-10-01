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
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMStudyMergedFilterProxyModel.h"

// STD includes
#include <iostream>

int ctkDICOMStudyMergedFilterProxyModelTest1(int argc, char* argv[])
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

    // Create multiple study models for different patients
    ctkDICOMStudyModel sourceModel1;
    sourceModel1.setDicomDatabase(database);
    sourceModel1.setPatientUID("Patient1");

    ctkDICOMStudyModel sourceModel2;
    sourceModel2.setDicomDatabase(database);
    sourceModel2.setPatientUID("Patient2");

    // Create filter proxy models
    ctkDICOMStudyFilterProxyModel proxyModel1;
    proxyModel1.setSourceModel(&sourceModel1);

    ctkDICOMStudyFilterProxyModel proxyModel2;
    proxyModel2.setSourceModel(&sourceModel2);

    // Create merged proxy model
    ctkDICOMStudyMergedFilterProxyModel mergedModel;

    // Test default state
    CHECK_INT(mergedModel.rowCount(), 0);

    // Set source proxy filter models
    QList<ctkDICOMStudyFilterProxyModel*> proxyModels;
    proxyModels << &proxyModel1 << &proxyModel2;
    mergedModel.setSourceProxyFilterModels(proxyModels);

    // Verify source models are set
    CHECK_INT(mergedModel.sourceProxyFilterModels().count(), 2);

    // Refresh models
    sourceModel1.refresh();
    sourceModel2.refresh();

    // Test that merged model combines rows from both sources
    int totalRows = proxyModel1.rowCount() + proxyModel2.rowCount();
    CHECK_INT(mergedModel.rowCount(), totalRows);

    // Test clearing source models
    mergedModel.clearSourceProxyFilterModels();
    CHECK_INT(mergedModel.sourceProxyFilterModels().count(), 0);
    CHECK_INT(mergedModel.rowCount(), 0);

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
