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

// STD includes
#include <iostream>

int ctkDICOMStudyModelTest1(int argc, char* argv[])
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

    // Create study model
    ctkDICOMStudyModel model;
    model.setDicomDatabase(database);

    // Test default values
    CHECK_INT(model.rowCount(), 0);
    CHECK_QSTRING(model.patientUID(), "");
    CHECK_QSTRING(model.patientID(), "");
    CHECK_QSTRING(model.studyDescriptionFilter(), "");
    CHECK_INT(model.dateFilter(), ctkDICOMStudyModel::Any);
    CHECK_INT(model.numberOfOpenedStudies(), 2);
    CHECK_INT(model.thumbnailSize(), 128);

    // Test setting patient UID and ID
    model.setPatientUID("TestPatientUID");
    CHECK_QSTRING(model.patientUID(), "TestPatientUID");

    model.setPatientID("TestPatientID");
    CHECK_QSTRING(model.patientID(), "TestPatientID");

    // Test study description filter
    model.setStudyDescriptionFilter("TestStudy");
    CHECK_QSTRING(model.studyDescriptionFilter(), "TestStudy");

    // Test date filter
    model.setDateFilter(ctkDICOMStudyModel::Today);
    CHECK_INT(model.dateFilter(), ctkDICOMStudyModel::Today);

    model.setDateFilter(ctkDICOMStudyModel::LastWeek);
    CHECK_INT(model.dateFilter(), ctkDICOMStudyModel::LastWeek);

    // Test modality filter
    QStringList modalities;
    modalities << "CT" << "MR";
    model.setModalityFilter(modalities);
    CHECK_INT(model.modalityFilter().count(), 2);

    // Test series description filter
    model.setSeriesDescriptionFilter("TestSeries");
    CHECK_QSTRING(model.seriesDescriptionFilter(), "TestSeries");

    // Test allowed servers
    QStringList servers;
    servers << "Server1" << "Server2";
    model.setAllowedServers(servers);
    CHECK_INT(model.allowedServers().count(), 2);

    // Test number of opened studies
    model.setNumberOfOpenedStudies(5);
    CHECK_INT(model.numberOfOpenedStudies(), 5);

    // Test thumbnail size
    model.setThumbnailSize(256);
    CHECK_INT(model.thumbnailSize(), 256);

    // Test refresh
    model.refresh();

    // Test role names
    QHash<int, QByteArray> roles = model.roleNames();
    CHECK_BOOL(roles.contains(ctkDICOMStudyModel::StudyInstanceUIDRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMStudyModel::StudyDescriptionRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMStudyModel::StudyDateRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMStudyModel::IsCollapsedRole), true);

    // Test collapsed state
    model.setAllStudiesCollapsed(true);
    model.setAllStudiesCollapsed(false);

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
