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
#include <QSignalSpy>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMJob.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMStudyModel.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>

// STD includes
#include <iostream>

namespace
{

//------------------------------------------------------------------------------
/// Insert a study of one series and one instance, without file
void insertStudy(ctkDICOMDatabase& database, const QString& studyInstanceUID, const QString& studyDate)
{
  DcmDataset dataset;
  dataset.putAndInsertString(DCM_PatientName, "Sorted^Studies");
  dataset.putAndInsertString(DCM_PatientID, "SORTED");
  dataset.putAndInsertString(DCM_StudyInstanceUID, studyInstanceUID.toLatin1().constData());
  dataset.putAndInsertString(DCM_StudyDate, studyDate.toLatin1().constData());
  dataset.putAndInsertString(DCM_StudyTime, "120000");
  dataset.putAndInsertString(DCM_SeriesInstanceUID, (studyInstanceUID + ".1").toLatin1().constData());
  dataset.putAndInsertString(DCM_Modality, "MR");
  dataset.putAndInsertString(DCM_SOPClassUID, UID_MRImageStorage);
  dataset.putAndInsertString(DCM_SOPInstanceUID, (studyInstanceUID + ".1.1").toLatin1().constData());
  database.insert(&dataset, /*storeFile=*/false, /*generateThumbnail=*/false);
}

//------------------------------------------------------------------------------
/// Order of the studies reported by the model once the series of a study are queried
QString studiesSortedByDateAfterSeriesQuery(ctkDICOMStudyModel& model, const QString& studyInstanceUID)
{
  QSignalSpy studiesSortedByDateSpy(&model, SIGNAL(studiesSortedByDate(QStringList)));
  ctkDICOMJobDetail seriesQueried;
  seriesQueried.JobUID = "SeriesQueryJob";
  seriesQueried.JobType = ctkDICOMJobResponseSet::JobType::QuerySeries;
  seriesQueried.PatientID = model.patientID();
  seriesQueried.StudyInstanceUID = studyInstanceUID;
  seriesQueried.QueriedSeriesInstanceUIDs = QStringList() << studyInstanceUID + ".1";
  model.updateGUIFromScheduler(QVariant::fromValue(seriesQueried));
  if (studiesSortedByDateSpy.count() != 1)
  {
    return QString("%1 signals").arg(studiesSortedByDateSpy.count());
  }
  return studiesSortedByDateSpy.at(0).at(0).toStringList().join(",");
}

} // end of anonymous namespace

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
    // Studies are opened on their own by default: only the models of the patients
    // that are not the current one have this disabled, so that querying several
    // patients does not retrieve image data for all of them
    CHECK_BOOL(model.autoOpenStudiesEnabled(), true);
    // The series models created by this model prefetch the full series by default
    CHECK_BOOL(model.autoRetrieveFullSeries(), true);

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

    // Test automatic opening of the first studies
    model.setAutoOpenStudiesEnabled(false);
    CHECK_BOOL(model.autoOpenStudiesEnabled(), false);
    model.setAutoOpenStudiesEnabled(true);
    CHECK_BOOL(model.autoOpenStudiesEnabled(), true);
    // The series models created by this model prefetch the full series by default
    CHECK_BOOL(model.autoRetrieveFullSeries(), true);

    // Test the automatic prefetch of the full series, forwarded to the series models
    model.setAutoRetrieveFullSeries(false);
    CHECK_BOOL(model.autoRetrieveFullSeries(), false);
    model.setAutoRetrieveFullSeries(true);
    CHECK_BOOL(model.autoRetrieveFullSeries(), true);

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

    //
    // The studies are sorted by date to open the most recent ones first. The order is
    // kept from one series query to the next: it must follow the studies added since,
    // and the dates that changed.
    //
    {
      ctkDICOMDatabase sortedDatabase;
      sortedDatabase.openDatabase(":memory:");
      const QString oldStudy("1.2.826.0.1.3680043.10.3.1");
      const QString recentStudy("1.2.826.0.1.3680043.10.3.2");
      insertStudy(sortedDatabase, oldStudy, "20100101");
      insertStudy(sortedDatabase, recentStudy, "20120101");

      ctkDICOMScheduler scheduler;
      ctkDICOMStudyModel sortedModel;
      sortedModel.setDicomDatabase(sortedDatabase);
      sortedModel.setScheduler(scheduler);
      CHECK_INT(sortedDatabase.patients().count(), 1);
      sortedModel.setPatientID("SORTED");
      sortedModel.setPatientUID(sortedDatabase.patients().first());
      sortedModel.refresh();
      CHECK_INT(sortedModel.rowCount(), 2);

      CHECK_QSTRING(studiesSortedByDateAfterSeriesQuery(sortedModel, oldStudy),
                    recentStudy + "," + oldStudy);

      // A study found by a later query
      const QString mostRecentStudy("1.2.826.0.1.3680043.10.3.3");
      insertStudy(sortedDatabase, mostRecentStudy, "20140101");
      sortedModel.refresh();
      CHECK_INT(sortedModel.rowCount(), 3);
      CHECK_QSTRING(studiesSortedByDateAfterSeriesQuery(sortedModel, oldStudy),
                    mostRecentStudy + "," + recentStudy + "," + oldStudy);

      // A date that changed
      CHECK_BOOL(sortedModel.setData(sortedModel.indexFromStudyInstanceUID(oldStudy),
                                     "20200101", ctkDICOMStudyModel::StudyDateRole), true);
      CHECK_QSTRING(studiesSortedByDateAfterSeriesQuery(sortedModel, recentStudy),
                    oldStudy + "," + mostRecentStudy + "," + recentStudy);
    }

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
