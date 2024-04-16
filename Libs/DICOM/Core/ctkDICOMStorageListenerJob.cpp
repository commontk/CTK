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
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// ctkCore includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMStorageListenerJob_p.h"
#include "ctkDICOMStorageListenerWorker.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMStorageListenerJob" );

//------------------------------------------------------------------------------
// ctkDICOMStorageListenerJobPrivate methods

//------------------------------------------------------------------------------
ctkDICOMStorageListenerJobPrivate::ctkDICOMStorageListenerJobPrivate(ctkDICOMStorageListenerJob* object)
 : q_ptr(object)
{
  this->AETitle = "CTKSTORE";
  this->Port = 11112;
  this->ConnectionTimeout = 1;
}

//------------------------------------------------------------------------------
ctkDICOMStorageListenerJobPrivate::~ctkDICOMStorageListenerJobPrivate() = default;

//------------------------------------------------------------------------------
// ctkDICOMStorageListenerJob methods

//------------------------------------------------------------------------------
ctkDICOMStorageListenerJob::ctkDICOMStorageListenerJob()
  : d_ptr(new ctkDICOMStorageListenerJobPrivate(this))
{
  this->Persistent = true;
}

//------------------------------------------------------------------------------
ctkDICOMStorageListenerJob::ctkDICOMStorageListenerJob(ctkDICOMStorageListenerJobPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMStorageListenerJob::~ctkDICOMStorageListenerJob() = default;

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMStorageListenerJob, const int&, setPort, Port);
CTK_GET_CPP(ctkDICOMStorageListenerJob, int, port, Port)
CTK_SET_CPP(ctkDICOMStorageListenerJob, const int&, setConnectionTimeout, ConnectionTimeout);
CTK_GET_CPP(ctkDICOMStorageListenerJob, int, connectionTimeout, ConnectionTimeout)
CTK_SET_CPP(ctkDICOMStorageListenerJob, const QString&, setAETitle, AETitle);
CTK_GET_CPP(ctkDICOMStorageListenerJob, QString, AETitle, AETitle)

//----------------------------------------------------------------------------
QString ctkDICOMStorageListenerJob::loggerReport(const QString& status) const
{
  return QString("ctkDICOMStorageListenerJob: listener job %1.\n"
                 "JobUID: %2\n")
                 .arg(status)
                 .arg(this->jobUID());
}
//------------------------------------------------------------------------------
ctkAbstractJob* ctkDICOMStorageListenerJob::clone() const
{
  ctkDICOMStorageListenerJob* newListenerJob = new ctkDICOMStorageListenerJob;
  newListenerJob->setAETitle(this->AETitle());
  newListenerJob->setPort(this->port());
  newListenerJob->setConnectionTimeout(this->connectionTimeout());
  newListenerJob->setMaximumNumberOfRetry(this->maximumNumberOfRetry());
  newListenerJob->setRetryDelay(this->retryDelay());
  newListenerJob->setRetryCounter(this->retryCounter());
  newListenerJob->setIsPersistent(this->isPersistent());
  newListenerJob->setMaximumConcurrentJobsPerType(this->maximumConcurrentJobsPerType());
  newListenerJob->setPriority(this->priority());

  return newListenerJob;
}

//------------------------------------------------------------------------------
ctkAbstractWorker* ctkDICOMStorageListenerJob::createWorker()
{
  ctkDICOMStorageListenerWorker* worker =
    new ctkDICOMStorageListenerWorker;
  worker->setJob(*this);
  return worker;
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::JobType ctkDICOMStorageListenerJob::getJobType() const
{
  return ctkDICOMJobResponseSet::JobType::StoreSOPInstance;
}
