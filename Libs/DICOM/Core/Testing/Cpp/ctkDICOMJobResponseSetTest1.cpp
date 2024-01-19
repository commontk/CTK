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
#include "ctkDICOMJobResponseSet.h"

int ctkDICOMJobResponseSetTest1(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  // Query Job and virtual parents (ctkDICOMJob and ctkAbstractJob)
  ctkDICOMJobResponseSet jobResponseSet;

  // Test the default values
  CHECK_QSTRING(jobResponseSet.filePath(), "");
  CHECK_BOOL(jobResponseSet.copyFile(), false);
  CHECK_BOOL(jobResponseSet.overwriteExistingDataset(), false);
  CHECK_INT(jobResponseSet.jobType(), ctkDICOMJobResponseSet::JobType::None);
  CHECK_QSTRING(jobResponseSet.jobUID(), "");
  CHECK_QSTRING(jobResponseSet.patientID(), "");
  CHECK_QSTRING(jobResponseSet.studyInstanceUID(), "");
  CHECK_QSTRING(jobResponseSet.seriesInstanceUID(), "");
  CHECK_QSTRING(jobResponseSet.sopInstanceUID(), "");
  CHECK_QSTRING(jobResponseSet.connectionName(), "");

  // Test setting and getting
  jobResponseSet.setFilePath("filePath");
  CHECK_QSTRING(jobResponseSet.filePath(), "filePath");
  jobResponseSet.setCopyFile(true);
  CHECK_BOOL(jobResponseSet.copyFile(), true);
  jobResponseSet.setOverwriteExistingDataset(true);
  CHECK_BOOL(jobResponseSet.overwriteExistingDataset(), true);
  jobResponseSet.setJobType(ctkDICOMJobResponseSet::JobType::RetrieveStudy);
  CHECK_INT(jobResponseSet.jobType(), ctkDICOMJobResponseSet::JobType::RetrieveStudy);
  jobResponseSet.setJobUID("JobUID");
  CHECK_QSTRING(jobResponseSet.jobUID(), "JobUID");
  jobResponseSet.setPatientID("patientID");
  CHECK_QSTRING(jobResponseSet.patientID(), "patientID");
  jobResponseSet.setStudyInstanceUID("studyInstanceUID");
  CHECK_QSTRING(jobResponseSet.studyInstanceUID(), "studyInstanceUID");
  jobResponseSet.setSeriesInstanceUID("seriesInstanceUID");
  CHECK_QSTRING(jobResponseSet.seriesInstanceUID(), "seriesInstanceUID");
  jobResponseSet.setSOPInstanceUID("sopInstanceUID");
  CHECK_QSTRING(jobResponseSet.sopInstanceUID(), "sopInstanceUID");
  jobResponseSet.setConnectionName("connectionName");
  CHECK_QSTRING(jobResponseSet.connectionName(), "connectionName");

  return EXIT_SUCCESS;
}
