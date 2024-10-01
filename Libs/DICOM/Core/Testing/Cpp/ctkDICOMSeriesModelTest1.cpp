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
#include <QDir>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMSeriesModel.h"

// STD includes
#include <iostream>

int ctkDICOMSeriesModelTest1(int argc, char* argv[])
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
    ctkDICOMSeriesModel model;
    model.setDicomDatabase(database);

    // Test default values
    CHECK_INT(model.rowCount(), 0);
    CHECK_INT(model.columnCount(), 1);
    CHECK_QSTRING(model.patientID(), "");
    CHECK_QSTRING(model.studyFilter(), "");
    CHECK_INT(model.thumbnailSize(), 128);
    CHECK_BOOL(model.autoGenerateThumbnails(), false);

    // Test setting patient ID and study filter
    model.setPatientID("TestPatient");
    CHECK_QSTRING(model.patientID(), "TestPatient");

    model.setStudyFilter("TestStudy");
    CHECK_QSTRING(model.studyFilter(), "TestStudy");

    // Test modality filter
    QStringList modalities;
    modalities << "CT" << "MR";
    model.setModalityFilter(modalities);
    CHECK_INT(model.modalityFilter().count(), 2);
    CHECK_QSTRING(model.modalityFilter().at(0), "CT");
    CHECK_QSTRING(model.modalityFilter().at(1), "MR");

    // Test series description filter
    model.setSeriesDescriptionFilter("TestSeries");
    CHECK_QSTRING(model.seriesDescriptionFilter(), "TestSeries");

    // Test thumbnail size
    model.setThumbnailSize(256);
    CHECK_INT(model.thumbnailSize(), 256);

    // Test auto-generate thumbnails
    model.setAutoGenerateThumbnails(true);
    CHECK_BOOL(model.autoGenerateThumbnails(), true);

    // Test allowed servers
    QStringList servers;
    servers << "Server1" << "Server2";
    model.setAllowedServers(servers);
    CHECK_INT(model.allowedServers().count(), 2);

    // Test refresh
    model.refresh();

    // Test role names
    QHash<int, QByteArray> roles = model.roleNames();
    CHECK_BOOL(roles.contains(ctkDICOMSeriesModel::SeriesInstanceUIDRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMSeriesModel::SeriesNumberRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMSeriesModel::ModalityRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMSeriesModel::SeriesDescriptionRole), true);

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
