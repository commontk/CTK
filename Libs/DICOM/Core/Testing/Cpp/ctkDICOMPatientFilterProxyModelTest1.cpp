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
#include <QFont>
#include <QTemporaryDir>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMPatientFilterProxyModel.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>

// STD includes
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
QString patientID(int patient)
{
  return QString("PAT%1").arg(patient, 4, 10, QChar('0'));
}

//------------------------------------------------------------------------------
/// Insert the patients [first, first + count[ of one study each, in a single transaction
void insertPatients(ctkDICOMDatabase& database, int first, int count)
{
  QList<ctkDICOMDatabase::IndexingResult> indexingResults;
  for (int patient = first; patient < first + count; ++patient)
  {
    const QString studyInstanceUID = QString("1.2.826.0.1.3680043.10.11.%1").arg(patient + 1);
    ctkDICOMDatabase::IndexingResult indexingResult;
    indexingResult.filePath = QString("/no/file/%1.dcm").arg(studyInstanceUID);
    indexingResult.dataset = QSharedPointer<ctkDICOMItem>(new ctkDICOMItem);
    indexingResult.dataset->InitializeFromItem(newInstance(patientID(patient), studyInstanceUID),
                                               /*takeOwnership=*/true);
    indexingResult.copyFile = false;
    indexingResult.overwriteExistingDataset = false;
    indexingResults << indexingResult;
  }
  database.insert(indexingResults);
}

//------------------------------------------------------------------------------
QString patientUID(ctkDICOMDatabase& database, const QString& patientID)
{
  foreach (const QString& patientUID, database.patients())
  {
    if (database.fieldForPatient("PatientID", patientUID) == patientID)
    {
      return patientUID;
    }
  }
  return QString();
}

//------------------------------------------------------------------------------
/// The patient UIDs that \a proxyModel shows, sorted (the proxy model sorts the
/// patients on their insert time, which is the same for patients inserted together)
QStringList proxyPatientUIDs(const ctkDICOMPatientFilterProxyModel& proxyModel)
{
  QStringList patientUIDs;
  for (int row = 0; row < proxyModel.rowCount(); ++row)
  {
    patientUIDs << proxyModel.index(row, 0).data(ctkDICOMPatientModel::PatientUIDRole).toString();
  }
  patientUIDs.sort();
  return patientUIDs;
}

//------------------------------------------------------------------------------
/// The patient UIDs that a proxy model filtering \a sourceModel from scratch shows,
/// with the settings of \a proxyModel
QStringList expectedPatientUIDs(ctkDICOMPatientModel& sourceModel,
                                const ctkDICOMPatientFilterProxyModel& proxyModel)
{
  ctkDICOMPatientFilterProxyModel referenceModel;
  referenceModel.setDisplayMode(proxyModel.displayMode());
  referenceModel.setWidgetWidth(proxyModel.widgetWidth());
  referenceModel.setSpacing(proxyModel.spacing());
  referenceModel.setIconSize(proxyModel.iconSize());
  referenceModel.setMaxTextWidth(proxyModel.maxTextWidth());
  referenceModel.setViewWidgetFont(proxyModel.viewWidgetFont());
  referenceModel.setSourceModel(&sourceModel);
  return proxyPatientUIDs(referenceModel);
}

} // end of anonymous namespace

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

    //
    // Time of the filtering of many patients in TabMode, where only the first ones
    // fit the widget width.
    //
    QTemporaryDir temporaryDirectory;
    CHECK_BOOL(temporaryDirectory.isValid(), true);
    {
      ctkDICOMDatabase manyPatientsDatabase;
      manyPatientsDatabase.openDatabase(
        QDir(temporaryDirectory.path()).filePath("manyPatients.sql"), "proxy_many_patients");
      const int numberOfPatients = 2000;
      insertPatients(manyPatientsDatabase, 0, numberOfPatients);
      ctkDICOMPatientModel manyPatientsModel;
      manyPatientsModel.setDicomDatabase(manyPatientsDatabase);
      manyPatientsModel.refresh();

      ctkDICOMPatientFilterProxyModel tabsModel;
      tabsModel.setWidgetWidth(1200);
      QElapsedTimer timer;
      timer.start();
      tabsModel.setSourceModel(&manyPatientsModel);
      const int numberOfTabs = tabsModel.rowCount();
      const qint64 initialFiltering = timer.elapsed();
      CHECK_BOOL(numberOfTabs > 0 && numberOfTabs < numberOfPatients, true);

      // All the patients change (e.g. the filters of the model change)
      timer.restart();
      manyPatientsModel.refreshPatients();
      const qint64 allPatientsChanged = timer.elapsed();

      // One patient changes at a time (e.g. its series have been queried), after the
      // ones that fit, then among them
      const int numberOfChanges = 200;
      const QString lastPatientUID = manyPatientsModel.patientUID(manyPatientsModel.index(numberOfPatients - 1));
      const QString firstPatientUID = manyPatientsModel.patientUID(manyPatientsModel.index(0));
      timer.restart();
      for (int change = 0; change < numberOfChanges; ++change)
      {
        manyPatientsModel.refreshPatient(lastPatientUID);
      }
      const qint64 changesAfterTabs = timer.elapsed();
      timer.restart();
      for (int change = 0; change < numberOfChanges; ++change)
      {
        manyPatientsModel.refreshPatient(firstPatientUID);
      }
      const qint64 changesInTabs = timer.elapsed();

      // A single patient is shown (patient ID filter), and changes one at a time
      manyPatientsModel.setPatientIDFilter(patientID(numberOfPatients - 1));
      timer.restart();
      manyPatientsModel.refreshPatients();
      const qint64 filterOnePatient = timer.elapsed();
      CHECK_INT(tabsModel.rowCount(), 1);
      timer.restart();
      for (int change = 0; change < numberOfChanges; ++change)
      {
        manyPatientsModel.refreshPatient(lastPatientUID);
      }
      const qint64 changesOfFilteredPatient = timer.elapsed();

      // A patient is added (e.g. found by a query)
      insertPatients(manyPatientsDatabase, numberOfPatients, 1);
      timer.restart();
      manyPatientsModel.refresh();
      const qint64 addPatient = timer.elapsed();

      std::cout << "Filtering " << numberOfPatients << " patients in TabMode (" << numberOfTabs
                << " shown): initial " << initialFiltering << " ms, all patients changed "
                << allPatientsChanged << " ms, " << numberOfChanges << " changes of a patient after the tabs "
                << changesAfterTabs << " ms, in the tabs " << changesInTabs << " ms; filter on one patient "
                << filterOnePatient << " ms, " << numberOfChanges << " changes of the filtered patient "
                << changesOfFilteredPatient << " ms; add a patient " << addPatient << " ms" << std::endl;
      // A change of a patient filters all the patients again when it may change the
      // patients that fit (less than a tenth of a millisecond in a release build): far
      // from the time of a population of the model
      CHECK_BOOL(allPatientsChanged < 1000, true);
      CHECK_BOOL(changesInTabs < 1000, true);
      CHECK_BOOL(changesOfFilteredPatient < 1000, true);
    }

    //
    // In TabMode, the patients shown are the ones that fit the widget width. The rows
    // after the first one that does not fit were rejected without being measured
    // again, even when the rows before it had changed: a patient filtered on its
    // patient ID, or deleted and queried again (appended at the end of the model),
    // was not shown until the widget was resized, and the browser had no patient to
    // select.
    //
    {
      ctkDICOMDatabase tabsDatabase;
      tabsDatabase.openDatabase(QDir(temporaryDirectory.path()).filePath("tabs.sql"), "proxy_tabs");
      insertPatients(tabsDatabase, 0, 12);
      ctkDICOMPatientModel tabsSourceModel;
      tabsSourceModel.setDicomDatabase(tabsDatabase);
      tabsSourceModel.refresh();

      ctkDICOMPatientFilterProxyModel tabsModel;
      tabsModel.setWidgetWidth(600);
      tabsModel.setSourceModel(&tabsSourceModel);
      // Only a few patients fit
      CHECK_BOOL(tabsModel.rowCount() > 1 && tabsModel.rowCount() < 12, true);
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));

      // Filter on a patient that did not fit
      const QString filteredPatientID = patientID(10);
      tabsSourceModel.setPatientIDFilter(filteredPatientID);
      tabsSourceModel.refreshPatients();
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), QStringList() << patientUID(tabsDatabase, filteredPatientID));

      // Delete the patient, then insert it again, as a query does
      tabsDatabase.removePatient(patientUID(tabsDatabase, filteredPatientID));
      tabsSourceModel.refresh();
      CHECK_INT(tabsModel.rowCount(), 0);
      insertPatients(tabsDatabase, 10, 1);
      tabsSourceModel.refresh();
      const QString insertedPatientUID = patientUID(tabsDatabase, filteredPatientID);
      CHECK_INT(tabsSourceModel.indexFromPatientUID(insertedPatientUID).row(), 11);
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), QStringList() << insertedPatientUID);

      // Remove the filter: the first patients fit again
      tabsSourceModel.setPatientIDFilter("");
      tabsSourceModel.refreshPatients();
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));

      // Hide a patient that fits (e.g. its studies are filtered out): the next one fits
      const QString firstPatientUID = tabsSourceModel.patientUID(tabsSourceModel.index(0));
      tabsSourceModel.setData(tabsSourceModel.index(0), false, ctkDICOMPatientModel::IsVisibleRole);
      CHECK_BOOL(proxyPatientUIDs(tabsModel).contains(firstPatientUID), false);
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));
      tabsSourceModel.refreshPatients();
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));

      // Delete a patient that fits
      tabsDatabase.removePatient(firstPatientUID);
      tabsSourceModel.refresh();
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));

      // Back from ListMode, and with another font
      tabsModel.setDisplayMode(ctkDICOMPatientFilterProxyModel::ListMode);
      CHECK_INT(tabsModel.rowCount(), tabsSourceModel.rowCount());
      tabsModel.setDisplayMode(ctkDICOMPatientFilterProxyModel::TabMode);
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));
      QFont largeFont = tabsModel.viewWidgetFont();
      largeFont.setPointSize(largeFont.pointSize() * 3);
      const int numberOfTabs = tabsModel.rowCount();
      tabsModel.setViewWidgetFont(largeFont);
      CHECK_BOOL(tabsModel.rowCount() <= numberOfTabs, true);
      CHECK_QSTRINGLIST(proxyPatientUIDs(tabsModel), expectedPatientUIDs(tabsSourceModel, tabsModel));
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
