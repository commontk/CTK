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

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMInserterJob.h"
#include "ctkDICOMQueryJob.h"
#include "ctkDICOMRetrieveJob.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMStorageListenerJob.h"

int ctkDICOMJobTest1(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  // Query Job and virtual parents (ctkDICOMJob and ctkAbstractJob)
  ctkDICOMQueryJob queryJob;

  // Test the default values
  CHECK_INT(queryJob.status(), ctkAbstractJob::JobStatus::Initialized);
  CHECK_BOOL(queryJob.isPersistent(), false);
  CHECK_INT(queryJob.retryCounter(), 0);
  CHECK_INT(queryJob.retryDelay(), 100);
  CHECK_INT(queryJob.maximumNumberOfRetry(), 3);
  CHECK_INT(queryJob.maximumConcurrentJobsPerType(), 20);
  CHECK_INT(queryJob.priority(), QThread::Priority::LowPriority);
  CHECK_INT(queryJob.dicomLevel(), ctkDICOMJob::DICOMLevels::None);
  CHECK_QSTRING(queryJob.patientID(), "");
  CHECK_QSTRING(queryJob.studyInstanceUID(), "");
  CHECK_QSTRING(queryJob.seriesInstanceUID(), "");
  CHECK_QSTRING(queryJob.sopInstanceUID(), "");
  CHECK_INT(queryJob.maximumPatientsQuery(), 25);
  CHECK_POINTER(queryJob.server(), nullptr);

  // Test setting and getting
  queryJob.setStatus(ctkAbstractJob::JobStatus::Running);
  CHECK_INT(queryJob.status(), ctkAbstractJob::JobStatus::Running);
  queryJob.setIsPersistent(true);
  CHECK_BOOL(queryJob.isPersistent(), true);
  queryJob.setJobUID("JobUID");
  CHECK_QSTRING(queryJob.jobUID(), "JobUID");
  queryJob.setRetryCounter(3);
  CHECK_INT(queryJob.retryCounter(), 3);
  queryJob.setRetryDelay(300);
  CHECK_INT(queryJob.retryDelay(), 300);
  queryJob.setMaximumNumberOfRetry(5);
  CHECK_INT(queryJob.maximumNumberOfRetry(), 5);
  queryJob.setMaximumConcurrentJobsPerType(5);
  CHECK_INT(queryJob.maximumConcurrentJobsPerType(), 5);
  queryJob.setPriority(QThread::Priority::HighPriority);
  CHECK_INT(queryJob.priority(), QThread::Priority::HighPriority);
  queryJob.setDICOMLevel(ctkDICOMJob::DICOMLevels::Studies);
  CHECK_INT(queryJob.dicomLevel(), ctkDICOMJob::DICOMLevels::Studies);
  queryJob.setPatientID("patientID");
  CHECK_QSTRING(queryJob.patientID(), "patientID");
  queryJob.setStudyInstanceUID("studyInstanceUID");
  CHECK_QSTRING(queryJob.studyInstanceUID(), "studyInstanceUID");
  queryJob.setSeriesInstanceUID("seriesInstanceUID");
  CHECK_QSTRING(queryJob.seriesInstanceUID(), "seriesInstanceUID");
  queryJob.setSOPInstanceUID("sopInstanceUID");
  CHECK_QSTRING(queryJob.sopInstanceUID(), "sopInstanceUID");
  queryJob.setMaximumPatientsQuery(100);
  CHECK_INT(queryJob.maximumPatientsQuery(), 100);
  ctkDICOMServer server;
  server.setConnectionName("server");
  queryJob.setServer(server);
  CHECK_QSTRING(queryJob.server()->connectionName(), "server");

  // Inserter Job
  ctkDICOMInserterJob inserterJob;

  // Test the default values
  CHECK_INT(inserterJob.maximumConcurrentJobsPerType(), 1);
  CHECK_QSTRING(inserterJob.databaseFilename(), "");
  QStringList tagsToPrecache;
  CHECK_QSTRINGLIST(inserterJob.tagsToPrecache(), tagsToPrecache)
  QStringList tagsToExcludeFromStorage;
  CHECK_QSTRINGLIST(inserterJob.tagsToExcludeFromStorage(), tagsToExcludeFromStorage)

  // Test setting and getting
  inserterJob.setDatabaseFilename("databaseFilename");
  CHECK_QSTRING(inserterJob.databaseFilename(), "databaseFilename");
  tagsToPrecache.append("tagsToPrecache");
  inserterJob.setTagsToPrecache(tagsToPrecache);
  CHECK_QSTRINGLIST(inserterJob.tagsToPrecache(), tagsToPrecache)
  tagsToExcludeFromStorage.append("tagsToExcludeFromStorage");
  inserterJob.setTagsToExcludeFromStorage(tagsToExcludeFromStorage);
  CHECK_QSTRINGLIST(inserterJob.tagsToExcludeFromStorage(), tagsToExcludeFromStorage)

  ctkDICOMRetrieveJob retrieveJob;

  // Test the default values
  CHECK_POINTER(retrieveJob.server(), nullptr);

  // Test setting and getting
  retrieveJob.setServer(server);
  CHECK_QSTRING(retrieveJob.server()->connectionName(), "server");

  ctkDICOMStorageListenerJob storageListenerJob;

  // Test the default values
  CHECK_INT(storageListenerJob.port(), 11112);
  CHECK_QSTRING(storageListenerJob.AETitle(), "CTKSTORE");
  CHECK_INT(storageListenerJob.connectionTimeout(), 1);

  // Test setting and getting
  storageListenerJob.setPort(80);
  CHECK_INT(storageListenerJob.port(), 80);
  storageListenerJob.setAETitle("AETitle");
  CHECK_QSTRING(storageListenerJob.AETitle(), "AETitle");
  storageListenerJob.setConnectionTimeout(5);
  CHECK_INT(storageListenerJob.connectionTimeout(), 5);

  return EXIT_SUCCESS;
}
