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

// ctk includes
#include "ctkCoreTestingMacros.h"

// ctkDICOMCore includes
#include "ctkDICOMQuery.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMTester.h"

int ctkDICOMSchedulerTest1(int argc, char * argv []) {

  QCoreApplication app(argc, argv);

  // run tester server
  ctkDICOMTester tester;
  std::cerr << "ctkDICOMSchedulerTest1: Starting dcmqrscp\n";
  tester.startDCMQRSCP();
  QStringList arguments = app.arguments();
  arguments.pop_front(); // remove test name
  int numberOfImages = arguments.count();
  if (!numberOfImages)
    {
    return EXIT_FAILURE;
    }
  std::cerr << "ctkDICOMSchedulerTest1: Storing data to dcmqrscp\n";
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
  std::cerr << "ctkDICOMSchedulerTest1: Setting up scheduler\n";
  ctkDICOMDatabase database;
  QString dbFile = "./ctkDICOM.sql";
  QFile file(dbFile);
  file.remove();
  QFile cacheFile("./ctkDICOMTagCache.sql");
  cacheFile.remove();
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

  std::cerr << "ctkDICOMSchedulerTest1: Running queryStudies\n";
  QString patientID = "Facial Expression";
  scheduler.queryStudies(patientID);
  scheduler.waitForFinish();

  CHECK_INT(database.patients().count(), 1);

  QString patientItem = database.patients()[0];
  QStringList studies = database.studiesForPatient(patientItem);
  CHECK_INT(studies.count(), 1);

  std::cerr << "ctkDICOMSchedulerTest1: Running querySeries\n";
  QString studyIstanceUID = studies[0];
  scheduler.querySeries(patientID, studyIstanceUID);
  scheduler.waitForFinish();

  QStringList series = database.seriesForStudy(studyIstanceUID);
  CHECK_INT(series.count(), 1);

  std::cerr << "ctkDICOMSchedulerTest1: Running queryInstances\n";
  QString seriesIstanceUID = series[0];
  scheduler.queryInstances(patientID, studyIstanceUID, seriesIstanceUID);
  scheduler.waitForFinish();

  QStringList instances = database.instancesForSeries(seriesIstanceUID);
  QStringList files = database.filesForSeries(seriesIstanceUID);
  files.removeAll(QString(""));
  QStringList urls = database.urlsForSeries(seriesIstanceUID);
  urls.removeAll(QString(""));

  CHECK_INT(instances.count(),numberOfImages);
  CHECK_INT(files.count(), 0);
  CHECK_INT(urls.count(), numberOfImages);

  std::cerr << "ctkDICOMSchedulerTest1: Running multiple retrieveSOPInstance."
               " This will test " << numberOfImages << " retrieve concorrent jobs\n";
  foreach (QString sopIstanceUID, instances)
    {
    scheduler.retrieveSOPInstance(patientID, studyIstanceUID, seriesIstanceUID, sopIstanceUID);
    }

  CHECK_INT(scheduler.numberOfJobs(), numberOfImages);
  scheduler.waitForFinish();

  instances = database.instancesForSeries(seriesIstanceUID);
  files = database.filesForSeries(seriesIstanceUID);
  files.removeAll(QString(""));
  urls = database.urlsForSeries(seriesIstanceUID);
  urls.removeAll(QString(""));

  CHECK_INT(instances.count(), numberOfImages);
  CHECK_INT(files.count(), numberOfImages);
  CHECK_INT(urls.count(), numberOfImages);

  return EXIT_SUCCESS;
}

