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
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

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
  CHECK_INT(queryJob.retryDelay(), 1000);
  CHECK_BOOL(queryJob.retryEnabled(), true);
  CHECK_INT(queryJob.maximumRetryWait(), 60000);
  CHECK_INT(queryJob.accumulatedRetryWait(), 0);
  CHECK_INT(queryJob.maximumConcurrentJobsPerType(), 20);
  CHECK_INT(queryJob.maximumConcurrentJobsPerGroup(), 8);
  // Without a server a job is not part of any concurrency group
  CHECK_QSTRING(queryJob.concurrencyGroup(), "");
  CHECK_INT(queryJob.priority(), QThread::Priority::LowPriority);
  CHECK_INT(queryJob.dicomLevel(), ctkDICOMJob::DICOMLevels::None);
  CHECK_QSTRING(queryJob.patientID(), "");
  CHECK_QSTRING(queryJob.studyInstanceUID(), "");
  CHECK_QSTRING(queryJob.seriesInstanceUID(), "");
  CHECK_QSTRING(queryJob.sopInstanceUID(), "");
  CHECK_INT(queryJob.maximumPatientsQuery(), 0);
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
  queryJob.setMaximumRetryWait(5000);
  CHECK_INT(queryJob.maximumRetryWait(), 5000);
  queryJob.setAccumulatedRetryWait(400);
  CHECK_INT(queryJob.accumulatedRetryWait(), 400);
  queryJob.setRetryEnabled(false);
  CHECK_BOOL(queryJob.retryEnabled(), false);
  queryJob.setRetryEnabled(true);
  queryJob.setMaximumConcurrentJobsPerType(5);
  CHECK_INT(queryJob.maximumConcurrentJobsPerType(), 5);
  queryJob.setMaximumConcurrentJobsPerGroup(2);
  CHECK_INT(queryJob.maximumConcurrentJobsPerGroup(), 2);
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
  // Jobs of a server compete with each other for that server's workers
  CHECK_QSTRING(queryJob.concurrencyGroup(), "server");

  // The retry settings survive the clone made for the next attempt
  QScopedPointer<ctkAbstractJob> clonedJob(queryJob.clone());
  CHECK_INT(clonedJob->maximumRetryWait(), 5000);
  CHECK_INT(clonedJob->retryDelay(), 300);
  CHECK_BOOL(clonedJob->retryEnabled(), true);
  CHECK_INT(clonedJob->maximumConcurrentJobsPerGroup(), 2);
  CHECK_QSTRING(qobject_cast<ctkDICOMQueryJob*>(clonedJob.data())->concurrencyGroup(), "server");
  CHECK_INT(qobject_cast<ctkDICOMQueryJob*>(clonedJob.data())->maximumPatientsQuery(), 100);

  // Inserter Job
  ctkDICOMInserterJob inserterJob;

  // Test the default values
  CHECK_INT(inserterJob.maximumConcurrentJobsPerType(), 20);
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
  CHECK_QSTRING(retrieveJob.concurrencyGroup(), "");
  CHECK_INT(retrieveJob.framesBatchLimit(), 25);

  // Test setting and getting
  retrieveJob.setServer(server);
  CHECK_QSTRING(retrieveJob.server()->connectionName(), "server");
  CHECK_QSTRING(retrieveJob.concurrencyGroup(), "server");
  retrieveJob.setFramesBatchLimit(120);
  CHECK_INT(retrieveJob.framesBatchLimit(), 120);

  // The batch size is kept when the job is reattempted
  QScopedPointer<ctkAbstractJob> clonedRetrieveJob(retrieveJob.clone());
  CHECK_INT(qobject_cast<ctkDICOMRetrieveJob*>(clonedRetrieveJob.data())->framesBatchLimit(), 120);
  CHECK_QSTRING(qobject_cast<ctkDICOMRetrieveJob*>(clonedRetrieveJob.data())->concurrencyGroup(), "server");

  // A frame is reported twice, on arrival and on insertion, and must be counted once.
  // Frames pulled by a C-GET arrive through the retrieve job, so they count on arrival.
  ctkDICOMJobResponseSet retrievedFrame;
  retrievedFrame.setJobType(ctkDICOMJobResponseSet::JobType::RetrieveSeries);
  CHECK_BOOL(ctkDICOMJobDetail(retrievedFrame).countsAsFrameProgress(), true);
  retrievedFrame.setInsertionCompleted(true);
  CHECK_BOOL(ctkDICOMJobDetail(retrievedFrame).countsAsFrameProgress(), false);

  // Frames pushed by a C-MOVE arrive at the storage listener, and the retrieve job
  // that asked for them only learns about them once they are inserted.
  ctkDICOMJobResponseSet storedFrame;
  storedFrame.setJobType(ctkDICOMJobResponseSet::JobType::StoreSOPInstance);
  CHECK_BOOL(ctkDICOMJobDetail(storedFrame).countsAsFrameProgress(), false);
  storedFrame.setInsertionCompleted(true);
  CHECK_BOOL(ctkDICOMJobDetail(storedFrame).countsAsFrameProgress(), true);

  // The flag survives the copy the inserter job works on
  QScopedPointer<ctkDICOMJobResponseSet> clonedFrame(storedFrame.clone());
  CHECK_BOOL(clonedFrame->insertionCompleted(), true);

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
