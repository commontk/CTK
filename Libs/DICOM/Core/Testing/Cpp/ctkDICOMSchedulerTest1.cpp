/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=============================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QTemporaryDir>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMTester.h"

int ctkDICOMSchedulerTest1(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  if (!arguments.count())
  {
    std::cerr << "Usage: " << qPrintable(testName)
              << " <path-to-image> [...]" << std::endl;
    return EXIT_FAILURE;
  }

  QTemporaryDir tempDirectory;
  CHECK_BOOL(tempDirectory.isValid(), true);

  int numberOfImages = arguments.count();

  ctkDICOMTester tester;

  std::cout << qPrintable(testName) << ": Starting dcmqrscp" << std::endl;
  tester.startDCMQRSCP();

  std::cout << qPrintable(testName) << ": Storing data to dcmqrscp" << std::endl;
  tester.storeData(arguments);

  ctkDICOMScheduler scheduler;

  // Test the default values
  CHECK_INT(scheduler.maximumThreadCount(), 20);
  CHECK_INT(scheduler.maximumNumberOfRetry(), 3);
  CHECK_INT(scheduler.retryDelay(), 100);
  CHECK_INT(scheduler.maximumPatientsQuery(), 25);

  // Test setting and getting
  scheduler.setMaximumThreadCount(19);
  CHECK_INT(scheduler.maximumThreadCount(), 19);
  scheduler.setMaximumNumberOfRetry(5);
  CHECK_INT(scheduler.maximumNumberOfRetry(), 5);
  scheduler.setRetryDelay(300);
  CHECK_INT(scheduler.retryDelay(), 300);
  scheduler.setMaximumPatientsQuery(30);
  CHECK_INT(scheduler.maximumPatientsQuery(), 30);

  // Test scheduler
  std::cout << qPrintable(testName) << ": Setting up scheduler" << std::endl;
  ctkDICOMDatabase database;

  QDir databaseDirectory(tempDirectory.path());
  QString dbFile = QFileInfo(databaseDirectory, QString("ctkDICOM.sql")).absoluteFilePath();
  CHECK_BOOL(database.openDatabase(dbFile), true);
  CHECK_BOOL(database.isOpen(), true);
  database.cleanup(true);

  CHECK_INT(database.patients().count(), 0);

  scheduler.setDicomDatabase(database);

  ctkDICOMServer server;
  server.setConnectionName("Test");
  server.setCallingAETitle("CTK_AE");
  server.setCalledAETitle("CTK_AE");
  server.setHost("localhost");
  server.setPort(tester.dcmqrscpPort());
  server.setRetrieveProtocol(ctkDICOMServer::RetrieveProtocol::CGET);

  scheduler.addServer(server);

  QMap<QString, QVariant> filsers;
  filsers.insert("ID", "Facial");
  scheduler.setFilters(filsers);

  std::cout << qPrintable(testName) << ": Running queryStudies" << std::endl;
  QString patientID = "Facial Expression";
  scheduler.queryStudies(patientID, QThread::LowPriority, QStringList("Test"));
  scheduler.waitForFinish(false, true);

  CHECK_INT(database.patients().count(), 1);

  QString patientItem = database.patients().at(0);
  QStringList studies = database.studiesForPatient(patientItem);
  CHECK_INT(studies.count(), 1);

  std::cout << qPrintable(testName) << ": Running querySeries" << std::endl;
  QString studyIstanceUID = studies[0];
  scheduler.querySeries(patientID, studyIstanceUID, QThread::LowPriority, QStringList("Test"));
  scheduler.waitForFinish(false, true);

  QStringList series = database.seriesForStudy(studyIstanceUID);
  CHECK_INT(series.count(), 1);

  std::cout << qPrintable(testName) << ": Running queryInstances" << std::endl;
  QString seriesIstanceUID = series[0];
  scheduler.queryInstances(patientID, studyIstanceUID, seriesIstanceUID, QThread::LowPriority, QStringList("Test"));
  scheduler.waitForFinish(false, true);

  QStringList instances = database.instancesForSeries(seriesIstanceUID);
  QStringList files = database.filesForSeries(seriesIstanceUID);
  files.removeAll(QString(""));
  QStringList urls = database.urlsForSeries(seriesIstanceUID);
  urls.removeAll(QString(""));

  CHECK_INT(instances.count(), numberOfImages);
  CHECK_INT(files.count(), 0);
  CHECK_INT(urls.count(), numberOfImages);

  std::cout << qPrintable(testName) << ": "
            << "Running multiple retrieveSOPInstance. "
            << "This will test " << numberOfImages << " retrieve concorrent jobs" << std::endl;

  foreach (const QString& sopIstanceUID, instances)
  {
    scheduler.retrieveSOPInstance(patientID, studyIstanceUID, seriesIstanceUID,
      sopIstanceUID, QThread::LowPriority, QStringList("Test"));
  }

  CHECK_INT(scheduler.numberOfRunningJobs(), numberOfImages);
  scheduler.waitForFinish(false, true);

  instances = database.instancesForSeries(seriesIstanceUID);
  files = database.filesForSeries(seriesIstanceUID);
  files.removeAll(QString(""));
  urls = database.urlsForSeries(seriesIstanceUID);
  urls.removeAll(QString(""));

  CHECK_INT(instances.count(), numberOfImages);
  CHECK_INT(files.count(), numberOfImages);
  CHECK_INT(urls.count(), 0);

  return EXIT_SUCCESS;
}
