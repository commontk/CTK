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
#include "ctkDICOMJobResponseSet.h" // For ctkDICOMJobDetail
#include "ctkDICOMEchoJob_p.h"
#include "ctkDICOMEchoWorker.h"
#include "ctkDICOMServer.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMRetrieveJob" );

//------------------------------------------------------------------------------
// ctkDICOMEchoJobPrivate methods

//------------------------------------------------------------------------------
ctkDICOMEchoJobPrivate::ctkDICOMEchoJobPrivate(ctkDICOMEchoJob* object)
 : q_ptr(object)
{
  this->Server = nullptr;
}

//------------------------------------------------------------------------------
ctkDICOMEchoJobPrivate::~ctkDICOMEchoJobPrivate()
{
  if (this->Server)
    {
    delete this->Server;
    this->Server = nullptr;
    }
}

//------------------------------------------------------------------------------
// ctkDICOMEchoJob methods

//------------------------------------------------------------------------------
ctkDICOMEchoJob::ctkDICOMEchoJob()
  : d_ptr(new ctkDICOMEchoJobPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMEchoJob::ctkDICOMEchoJob(ctkDICOMEchoJobPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMEchoJob::~ctkDICOMEchoJob() = default;

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMEchoJob::server() const
{
  Q_D(const ctkDICOMEchoJob);
  return d->Server;
}

//----------------------------------------------------------------------------
void ctkDICOMEchoJob::setServer(const ctkDICOMServer& server)
{
  Q_D(ctkDICOMEchoJob);
  d->Server = server.clone();
}

//----------------------------------------------------------------------------
QString ctkDICOMEchoJob::loggerReport(const QString& status) const
{
  return QString("ctkDICOMEchoJob: echo job %1.\n")
      .arg(status);
}
//------------------------------------------------------------------------------
ctkAbstractJob* ctkDICOMEchoJob::clone() const
{
  ctkDICOMEchoJob* newEchoJob = new ctkDICOMEchoJob;
  newEchoJob->setServer(*this->server());
  newEchoJob->setMaximumNumberOfRetry(this->maximumNumberOfRetry());
  newEchoJob->setRetryDelay(this->retryDelay());
  newEchoJob->setRetryCounter(this->retryCounter());
  newEchoJob->setIsPersistent(this->isPersistent());
  newEchoJob->setMaximumConcurrentJobsPerType(this->maximumConcurrentJobsPerType());
  newEchoJob->setPriority(this->priority());

  return newEchoJob;
}

//------------------------------------------------------------------------------
ctkAbstractWorker* ctkDICOMEchoJob::createWorker()
{
  ctkDICOMEchoWorker* worker =
    new ctkDICOMEchoWorker;
  worker->setJob(*this);
  return worker;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMEchoJob::toVariant()
{
  return QVariant::fromValue(ctkDICOMJobDetail(*this, this->server()->connectionName()));
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::JobType ctkDICOMEchoJob::getJobType() const
{
  return ctkDICOMJobResponseSet::JobType::Echo;
}
