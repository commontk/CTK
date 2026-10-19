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
#include <QElapsedTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QThread>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMItem.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMStudyModel.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>

// STD includes
#include <atomic>
#include <iostream>

namespace
{

//------------------------------------------------------------------------------
/// An instance of a study of one series, without file
DcmDataset* newInstance(const QString& patientID, const QString& studyInstanceUID)
{
  DcmDataset* dataset = new DcmDataset;
  dataset->putAndInsertString(DCM_PatientName, ("Patient^" + patientID).toLatin1().constData());
  dataset->putAndInsertString(DCM_PatientID, patientID.toLatin1().constData());
  dataset->putAndInsertString(DCM_StudyInstanceUID, studyInstanceUID.toLatin1().constData());
  dataset->putAndInsertString(DCM_StudyDate, "20200101");
  dataset->putAndInsertString(DCM_SeriesInstanceUID, (studyInstanceUID + ".1").toLatin1().constData());
  dataset->putAndInsertString(DCM_Modality, "MR");
  dataset->putAndInsertString(DCM_SOPClassUID, UID_MRImageStorage);
  dataset->putAndInsertString(DCM_SOPInstanceUID, (studyInstanceUID + ".1.1").toLatin1().constData());
  return dataset;
}

//------------------------------------------------------------------------------
/// Insert patients of two studies each, in a single transaction
void insertPatients(ctkDICOMDatabase& database, int numberOfPatients)
{
  QList<ctkDICOMDatabase::IndexingResult> indexingResults;
  for (int patient = 0; patient < numberOfPatients; ++patient)
  {
    for (int study = 0; study < 2; ++study)
    {
      const QString studyInstanceUID = QString("1.2.826.0.1.3680043.10.7.%1.%2").arg(patient + 1).arg(study + 1);
      ctkDICOMDatabase::IndexingResult indexingResult;
      indexingResult.filePath = QString("/no/file/%1.dcm").arg(studyInstanceUID);
      indexingResult.dataset = QSharedPointer<ctkDICOMItem>(new ctkDICOMItem);
      indexingResult.dataset->InitializeFromItem(newInstance(QString("PAT%1").arg(patient + 1), studyInstanceUID),
                                                 /*takeOwnership=*/true);
      indexingResult.copyFile = false;
      indexingResult.overwriteExistingDataset = false;
      indexingResults << indexingResult;
    }
  }
  database.insert(indexingResults);
}

//------------------------------------------------------------------------------
/// The IDs of the patients shown, sorted
QStringList shownPatients(ctkDICOMPatientModel& model)
{
  QStringList patientIDs;
  for (int row = 0; row < model.rowCount(); ++row)
  {
    const QModelIndex index = model.index(row, 0);
    if (model.isPatientVisible(index))
    {
      patientIDs << model.patientID(index);
    }
  }
  patientIDs.sort();
  return patientIDs;
}

//------------------------------------------------------------------------------
/// Writes to a database from a connection of its own, as the import and retrieve
/// jobs do, and records the longest write.
class ctkDICOMPatientModelWriterThread : public QThread
{
public:
  ctkDICOMPatientModelWriterThread(const QString& databaseFilePath)
    : DatabaseFilePath(databaseFilePath)
  {
  }

  void run() override
  {
    ctkDICOMDatabase database;
    database.openDatabase(this->DatabaseFilePath, "patient_model_writer");
    int write = 0;
    while (!this->Stop)
    {
      const QString studyInstanceUID = QString("1.2.826.0.1.3680043.10.8.%1").arg(++write);
      DcmDataset* dataset = newInstance(QString("WRITER%1").arg(write), studyInstanceUID);
      QElapsedTimer timer;
      timer.start();
      database.insert(dataset, /*storeFile=*/false, /*generateThumbnail=*/false);
      const qint64 duration = timer.elapsed();
      delete dataset;
      if (duration > this->LongestWrite)
      {
        this->LongestWrite = duration;
      }
      this->Writes = write;
      QThread::msleep(1);
    }
    database.closeDatabase();
  }

  QString DatabaseFilePath;
  std::atomic<bool> Stop{false};
  std::atomic<int> Writes{0};
  std::atomic<qint64> LongestWrite{0};
};

} // end of anonymous namespace

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
    // Series are prefetched in full by default, and no patient is current until one
    // is selected, so a query does not retrieve image data for every patient it found
    CHECK_BOOL(model.autoRetrieveFullSeries(), true);
    CHECK_QSTRING(model.currentPatientUID(), "");

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

    // Test the automatic prefetch of the full series, forwarded to the study models
    model.setAutoRetrieveFullSeries(false);
    CHECK_BOOL(model.autoRetrieveFullSeries(), false);
    model.setAutoRetrieveFullSeries(true);
    CHECK_BOOL(model.autoRetrieveFullSeries(), true);

    // Test the current patient
    model.setCurrentPatientUID("TestPatientUID");
    CHECK_QSTRING(model.currentPatientUID(), "TestPatientUID");

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

    //
    // A database of many patients is populated in a short while: the models of every
    // patient, study and series are built, but not their filter proxy models, and the
    // database is read in transactions. A browser populated it several times in a row
    // while it was set up, which took seconds before the browser was shown.
    //
    QTemporaryDir manyPatientsDirectory;
    CHECK_BOOL(manyPatientsDirectory.isValid(), true);
    const QString manyPatientsFile = QDir(manyPatientsDirectory.path()).filePath("ctkDICOM.sql");
    const int numberOfPatients = 2000;
    {
      ctkDICOMDatabase manyPatientsDatabase;
      manyPatientsDatabase.openDatabase(manyPatientsFile, "patient_model_many_patients");
      insertPatients(manyPatientsDatabase, numberOfPatients);
      CHECK_INT(manyPatientsDatabase.patients().count(), numberOfPatients);

      ctkDICOMPatientModel manyPatientsModel;
      manyPatientsModel.setDicomDatabase(manyPatientsDatabase);
      QElapsedTimer timer;
      timer.start();
      manyPatientsModel.refresh();
      const qint64 duration = timer.elapsed();
      std::cout << "Populating " << numberOfPatients << " patients of 2 studies took "
                << duration << " ms" << std::endl;
      CHECK_INT(manyPatientsModel.rowCount(), numberOfPatients);
      CHECK_INT(manyPatientsModel.allStudyModels().count(), numberOfPatients);
      // A few hundred milliseconds in a debug build, against twice as much when each
      // statement locked the database file again and a filter proxy model was created
      // for each patient and study
      CHECK_BOOL(duration < 3000, true);

      // The filter proxy models are created when asked for, once
      CHECK_INT(manyPatientsModel.findChildren<ctkDICOMStudyFilterProxyModel*>().count(), 0);
      const QString patientUID = manyPatientsModel.patientUID(manyPatientsModel.index(0));
      ctkDICOMStudyFilterProxyModel* studyFilterProxyModel =
        manyPatientsModel.studyFilterProxyModelForPatientUID(patientUID);
      CHECK_NOT_NULL(studyFilterProxyModel);
      CHECK_POINTER(studyFilterProxyModel->sourceModel(), manyPatientsModel.studyModelForPatientUID(patientUID));
      CHECK_POINTER(manyPatientsModel.studyFilterProxyModelForPatientUID(patientUID), studyFilterProxyModel);
      CHECK_INT(manyPatientsModel.findChildren<ctkDICOMStudyFilterProxyModel*>().count(), 1);
      CHECK_NULL(manyPatientsModel.studyFilterProxyModelForPatientUID("not.a.patient"));
      CHECK_INT(manyPatientsModel.allStudyFilterProxyModels().count(), numberOfPatients);

      // No read transaction is left open, which would keep the other connections from
      // writing to the database
      QSqlDatabase readDatabase = manyPatientsDatabase.database();
      CHECK_BOOL(readDatabase.transaction(), true);
      readDatabase.rollback();
    }

    //
    // The writers of the database (e.g. the import and retrieve jobs, each on a
    // connection of its own) wait a short while at most while the models are
    // populated: the read transaction is renewed every so many patients, instead of
    // holding the database for the whole population.
    //
    {
      ctkDICOMDatabase readDatabase;
      readDatabase.openDatabase(manyPatientsFile, "patient_model_reader");
      ctkDICOMPatientModelWriterThread writer(manyPatientsFile);
      writer.start();
      while (writer.Writes < 5)
      {
        QThread::msleep(1);
      }
      writer.LongestWrite = 0;
      qint64 shortestPopulation = -1;
      for (int population = 0; population < 3; ++population)
      {
        ctkDICOMPatientModel populatedModel;
        populatedModel.setDicomDatabase(readDatabase);
        QElapsedTimer timer;
        timer.start();
        populatedModel.refresh();
        const qint64 duration = timer.elapsed();
        CHECK_BOOL(populatedModel.rowCount() >= numberOfPatients, true);
        if (shortestPopulation < 0 || duration < shortestPopulation)
        {
          shortestPopulation = duration;
        }
      }
      writer.Stop = true;
      writer.wait();
      std::cout << "Longest write during a population: " << writer.LongestWrite
                << " ms, shortest population: " << shortestPopulation << " ms, "
                << writer.Writes << " writes" << std::endl;
      // A write waits for a part of the population, not for all of it
      CHECK_BOOL(writer.LongestWrite < shortestPopulation / 2, true);
    }

    //
    // The patients shown by the filters. A patient whose studies were found by a query
    // has no series until they are queried: it was hidden, and so were all the patients
    // of a database of such patients. A patient found by a query has no study yet.
    //
    {
      QTemporaryDir visibilityDirectory;
      CHECK_BOOL(visibilityDirectory.isValid(), true);
      ctkDICOMDatabase visibilityDatabase;
      visibilityDatabase.openDatabase(
        QDir(visibilityDirectory.path()).filePath("ctkDICOM.sql"), "patient_model_visibility");
      // PAT1: two studies of one MR series each; PAT2: two PET studies without series;
      // PAT3: no study
      insertPatients(visibilityDatabase, 3);
      QSqlQuery sql(visibilityDatabase.database());
      CHECK_BOOL(sql.exec("UPDATE Series SET SeriesDescription = 'Axial T1' WHERE StudyInstanceUID LIKE '%.7.1.%'"), true);
      CHECK_BOOL(sql.exec("UPDATE Studies SET StudyDescription = 'Neuro PET', ModalitiesInStudy = 'PT' "
                          "WHERE StudyInstanceUID LIKE '%.7.2.%'"), true);
      CHECK_BOOL(sql.exec("DELETE FROM Images WHERE SeriesInstanceUID LIKE '%.7.2.%' OR SeriesInstanceUID LIKE '%.7.3.%'"), true);
      CHECK_BOOL(sql.exec("DELETE FROM Series WHERE StudyInstanceUID LIKE '%.7.2.%' OR StudyInstanceUID LIKE '%.7.3.%'"), true);
      CHECK_BOOL(sql.exec("DELETE FROM Studies WHERE StudyInstanceUID LIKE '%.7.3.%'"), true);

      ctkDICOMPatientModel visibilityModel;
      visibilityModel.setDicomDatabase(visibilityDatabase);
      visibilityModel.refresh();
      CHECK_INT(visibilityModel.rowCount(), 3);
      // No filter: all the patients, with all their studies
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT1" << "PAT2" << "PAT3");
      const QModelIndex pet = visibilityModel.indexFromPatientID("PAT2");
      CHECK_INT(pet.data(ctkDICOMPatientModel::StudyCountRole).toInt(), 2);
      CHECK_INT(pet.data(ctkDICOMPatientModel::FilteredStudyCountRole).toInt(), 2);
      CHECK_INT(pet.data(ctkDICOMPatientModel::SeriesCountRole).toInt(), 0);

      // A series description cannot be checked on series that are not known yet
      visibilityModel.setSeriesDescriptionFilter("axial");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT1");
      visibilityModel.setSeriesDescriptionFilter("nothing");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList());
      visibilityModel.setSeriesDescriptionFilter("");

      // A modality is checked on the series, or on the modalities of the study
      visibilityModel.setModalityFilter(QStringList() << "PT");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT2");
      visibilityModel.setModalityFilter(QStringList() << "MR");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT1");
      visibilityModel.setModalityFilter(QStringList() << "CT");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList());
      visibilityModel.setModalityFilter(QStringList() << "Any");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT1" << "PAT2" << "PAT3");

      // A study description is checked on the studies, with series or not
      visibilityModel.setStudyDescriptionFilter("neuro");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT2");
      CHECK_INT(pet.data(ctkDICOMPatientModel::FilteredStudyCountRole).toInt(), 2);
      visibilityModel.setStudyDescriptionFilter("");

      // The patient filters
      visibilityModel.setPatientIDFilter("PAT3");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT3");
      visibilityModel.setPatientIDFilter("");
      CHECK_QSTRINGLIST(shownPatients(visibilityModel), QStringList() << "PAT1" << "PAT2" << "PAT3");
    }

    //
    // The allowed servers of a patient, as the combo box of the patient view saves
    // them: it passes only the servers checked explicitly. The trusted servers that
    // are neither allowed nor denied explicitly (partially checked) stay allowed. They
    // were dropped from the allowed servers of the patient and of its study model when
    // another server was unchecked, so that the series could not be retrieved anymore
    // until the patient was queried again.
    //
    {
      QTemporaryDir allowedServersDirectory;
      CHECK_BOOL(allowedServersDirectory.isValid(), true);
      ctkDICOMDatabase allowedServersDatabase;
      allowedServersDatabase.openDatabase(
        QDir(allowedServersDirectory.path()).filePath("ctkDICOM.sql"), "patient_model_allowed_servers");
      insertPatients(allowedServersDatabase, 1);
      const QString patientUID = allowedServersDatabase.patients().first();

      // (the servers are not owned by the scheduler, they must outlive it)
      ctkDICOMServer trustedServer1;
      trustedServer1.setConnectionName("Trusted1");
      trustedServer1.setQueryRetrieveEnabled(true);
      trustedServer1.setTrustedEnabled(true);
      ctkDICOMServer trustedServer2;
      trustedServer2.setConnectionName("Trusted2");
      trustedServer2.setQueryRetrieveEnabled(true);
      trustedServer2.setTrustedEnabled(true);
      ctkDICOMServer untrustedServer;
      untrustedServer.setConnectionName("Untrusted");
      untrustedServer.setQueryRetrieveEnabled(true);
      untrustedServer.setTrustedEnabled(false);
      ctkDICOMScheduler scheduler;
      scheduler.setDicomDatabase(allowedServersDatabase);
      scheduler.addServer(trustedServer1);
      scheduler.addServer(trustedServer2);
      scheduler.addServer(untrustedServer);

      // The patient was fetched from the first trusted server
      allowedServersDatabase.updateConnectionsForPatient(patientUID, QStringList() << "Trusted1", QStringList());

      ctkDICOMPatientModel allowedServersModel;
      allowedServersModel.setDicomDatabase(allowedServersDatabase);
      allowedServersModel.setScheduler(scheduler);
      allowedServersModel.refresh();
      ctkDICOMStudyModel* studyModel = allowedServersModel.studyModelForPatientUID(patientUID);
      CHECK_NOT_NULL(studyModel);

      const QStringList trustedServers = QStringList() << "Trusted1" << "Trusted2";
      CHECK_QSTRINGLIST(allowedServersModel.allowedServers(patientUID), trustedServers);
      CHECK_QSTRINGLIST(studyModel->allowedServers(), trustedServers);

      // Uncheck the first trusted server: it is not denied, being trusted, and both
      // trusted servers stay allowed. The untrusted server, unchecked, is denied.
      allowedServersModel.saveAllowedServersToDB(patientUID, QStringList());
      QMap<QString, QStringList> connections = allowedServersDatabase.connectionsInformationForPatient(patientUID);
      CHECK_QSTRINGLIST(connections["allow"], QStringList());
      CHECK_QSTRINGLIST(connections["deny"], QStringList() << "Untrusted");
      CHECK_QSTRINGLIST(allowedServersModel.allowedServers(patientUID), trustedServers);
      CHECK_QSTRINGLIST(studyModel->allowedServers(), trustedServers);

      // Check the untrusted server: it is allowed explicitly, in addition to the
      // trusted servers
      allowedServersModel.saveAllowedServersToDB(patientUID, QStringList() << "Untrusted");
      connections = allowedServersDatabase.connectionsInformationForPatient(patientUID);
      CHECK_QSTRINGLIST(connections["allow"], QStringList() << "Untrusted");
      CHECK_QSTRINGLIST(connections["deny"], QStringList());
      CHECK_QSTRINGLIST(allowedServersModel.allowedServers(patientUID), QStringList(trustedServers) << "Untrusted");
      CHECK_QSTRINGLIST(studyModel->allowedServers(), QStringList(trustedServers) << "Untrusted");

      // Uncheck the untrusted server: it is denied explicitly
      allowedServersModel.saveAllowedServersToDB(patientUID, QStringList());
      connections = allowedServersDatabase.connectionsInformationForPatient(patientUID);
      CHECK_QSTRINGLIST(connections["allow"], QStringList());
      CHECK_QSTRINGLIST(connections["deny"], QStringList() << "Untrusted");
      CHECK_QSTRINGLIST(allowedServersModel.allowedServers(patientUID), trustedServers);
      CHECK_QSTRINGLIST(studyModel->allowedServers(), trustedServers);
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
