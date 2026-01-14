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

// STD includes
#include <iostream>

int ctkDICOMPatientModelTest1(int argc, char* argv[])
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
    ctkDICOMPatientModel model;
    model.setDicomDatabase(database);

    // Test default values
    CHECK_INT(model.rowCount(), 0);
    CHECK_QSTRING(model.patientIDFilter(), "");
    CHECK_QSTRING(model.patientNameFilter(), "");
    CHECK_QSTRING(model.studyDescriptionFilter(), "");
    CHECK_QSTRING(model.seriesDescriptionFilter(), "");
    CHECK_INT(model.dateFilter(), ctkDICOMPatientModel::Any);
    CHECK_INT(model.numberOfOpenedStudiesPerPatient(), 2);
    CHECK_INT(model.thumbnailSize(), 128);

    // Test patient ID filter
    model.setPatientIDFilter("TestPatient");
    CHECK_QSTRING(model.patientIDFilter(), "TestPatient");

    // Test patient name filter
    model.setPatientNameFilter("TestName");
    CHECK_QSTRING(model.patientNameFilter(), "TestName");

    // Test study description filter
    model.setStudyDescriptionFilter("TestStudy");
    CHECK_QSTRING(model.studyDescriptionFilter(), "TestStudy");

    // Test series description filter
    model.setSeriesDescriptionFilter("TestSeries");
    CHECK_QSTRING(model.seriesDescriptionFilter(), "TestSeries");

    // Test date filter
    model.setDateFilter(ctkDICOMPatientModel::Today);
    CHECK_INT(model.dateFilter(), ctkDICOMPatientModel::Today);

    // Test modality filter
    QStringList modalities;
    modalities << "CT" << "MR";
    model.setModalityFilter(modalities);
    CHECK_INT(model.modalityFilter().count(), 2);

    // Test number of opened studies
    model.setNumberOfOpenedStudiesPerPatient(5);
    CHECK_INT(model.numberOfOpenedStudiesPerPatient(), 5);

    // Test thumbnail size
    model.setThumbnailSize(256);
    CHECK_INT(model.thumbnailSize(), 256);

    // Test refresh
    model.refresh();

    // Test role names
    QHash<int, QByteArray> roles = model.roleNames();
    CHECK_BOOL(roles.contains(ctkDICOMPatientModel::PatientUIDRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMPatientModel::PatientIDRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMPatientModel::PatientNameRole), true);
    CHECK_BOOL(roles.contains(ctkDICOMPatientModel::StudyCountRole), true);

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
