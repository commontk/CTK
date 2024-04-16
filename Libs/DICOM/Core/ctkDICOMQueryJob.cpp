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
#include "ctkDICOMQueryJob_p.h"
#include "ctkDICOMQueryWorker.h"
#include "ctkDICOMServer.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMQueryJob" );

//------------------------------------------------------------------------------
// ctkDICOMQueryJobPrivate methods

//------------------------------------------------------------------------------
ctkDICOMQueryJobPrivate::ctkDICOMQueryJobPrivate(ctkDICOMQueryJob* object)
 : q_ptr(object)
{
  this->Server = nullptr;
  this->MaximumPatientsQuery = 25;
}

//------------------------------------------------------------------------------
ctkDICOMQueryJobPrivate::~ctkDICOMQueryJobPrivate()
{
  if (this->Server)
    {
    delete this->Server;
    this->Server = nullptr;
    }
}

//------------------------------------------------------------------------------
// ctkDICOMQueryJob methods

//------------------------------------------------------------------------------
ctkDICOMQueryJob::ctkDICOMQueryJob()
  : d_ptr(new ctkDICOMQueryJobPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMQueryJob::~ctkDICOMQueryJob() = default;

//------------------------------------------------------------------------------
ctkDICOMQueryJob::ctkDICOMQueryJob(ctkDICOMQueryJobPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMQueryJob, const int&, setMaximumPatientsQuery, MaximumPatientsQuery);
CTK_GET_CPP(ctkDICOMQueryJob, int, maximumPatientsQuery, MaximumPatientsQuery)

//----------------------------------------------------------------------------
void ctkDICOMQueryJob::setFilters(const QMap<QString, QVariant>& filters)
{
  Q_D(ctkDICOMQueryJob);
  d->Filters = filters;
}

//----------------------------------------------------------------------------
QMap<QString, QVariant> ctkDICOMQueryJob::filters() const
{
  Q_D(const ctkDICOMQueryJob);
  return d->Filters;
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMQueryJob::server() const
{
  Q_D(const ctkDICOMQueryJob);
  return d->Server;
}

//----------------------------------------------------------------------------
void ctkDICOMQueryJob::setServer(const ctkDICOMServer& server)
{
  Q_D(ctkDICOMQueryJob);
  d->Server = server.clone();
}

//----------------------------------------------------------------------------
QString ctkDICOMQueryJob::loggerReport(const QString& status) const
{
  switch (this->dicomLevel())
  {
    case ctkDICOMJob::DICOMLevels::Patients:
      return QString("ctkDICOMQueryJob: query job at patients level %1.\n"
                     "JobUID: %2\n"
                     "Server: %3\n")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->server()->connectionName());
    case ctkDICOMJob::DICOMLevels::Studies:
      return QString("ctkDICOMQueryJob: query job at studies level %1.\n"
                     "JobUID: %2\n"
                     "Server: %3\n"
                     "PatientID: %4\n")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->server()->connectionName())
          .arg(this->patientID());
    case ctkDICOMJob::DICOMLevels::Series:
      return QString("ctkDICOMQueryJob: query job at series level %1.\n"
                     "JobUID: %2\n"
                     "Server: %3\n"
                     "PatientID: %4\n"
                     "StudyInstanceUID: %5\n")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->server()->connectionName())
          .arg(this->patientID())
          .arg(this->studyInstanceUID());
    case ctkDICOMJob::DICOMLevels::Instances:
      return QString("ctkDICOMQueryJob: query job at instances level %1.\n"
                     "JobUID: %2\n"
                     "Server: %3\n"
                     "PatientID: %4\n"
                     "StudyInstanceUID: %5\n"
                     "SeriesInstanceUID: %6\n")
          .arg(status)
          .arg(this->jobUID())
          .arg(this->server()->connectionName())
          .arg(this->patientID())
          .arg(this->studyInstanceUID())
          .arg(this->seriesInstanceUID());
    default:
      return QString("");
  }
}

//------------------------------------------------------------------------------
ctkAbstractJob* ctkDICOMQueryJob::clone() const
{
  ctkDICOMQueryJob* newQueryJob = new ctkDICOMQueryJob;
  newQueryJob->setMaximumPatientsQuery(this->maximumConcurrentJobsPerType());
  newQueryJob->setServer(*this->server());
  newQueryJob->setFilters(this->filters());
  newQueryJob->setDICOMLevel(this->dicomLevel());
  newQueryJob->setPatientID(this->patientID());
  newQueryJob->setStudyInstanceUID(this->studyInstanceUID());
  newQueryJob->setSeriesInstanceUID(this->seriesInstanceUID());
  newQueryJob->setSOPInstanceUID(this->sopInstanceUID());
  newQueryJob->setMaximumNumberOfRetry(this->maximumNumberOfRetry());
  newQueryJob->setRetryDelay(this->retryDelay());
  newQueryJob->setRetryCounter(this->retryCounter());
  newQueryJob->setIsPersistent(this->isPersistent());
  newQueryJob->setMaximumConcurrentJobsPerType(this->maximumConcurrentJobsPerType());
  newQueryJob->setPriority(this->priority());

  return newQueryJob;
}

//------------------------------------------------------------------------------
ctkAbstractWorker* ctkDICOMQueryJob::createWorker()
{
  ctkDICOMQueryWorker* worker =
    new ctkDICOMQueryWorker;
  worker->setJob(*this);
  return worker;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMQueryJob::toVariant()
{
  return QVariant::fromValue(ctkDICOMJobDetail(*this, this->server()->connectionName()));
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::JobType ctkDICOMQueryJob::getJobType() const
{
  if (this->DICOMLevel == ctkDICOMJob::DICOMLevels::Patients)
  {
    return ctkDICOMJobResponseSet::JobType::QueryPatients;
  }
  else if (this->DICOMLevel == ctkDICOMJob::DICOMLevels::Studies)
  {
    return ctkDICOMJobResponseSet::JobType::QueryStudies;
  }
  else if (this->DICOMLevel == ctkDICOMJob::DICOMLevels::Series)
  {
    return ctkDICOMJobResponseSet::JobType::QuerySeries;
  }
  else if (this->DICOMLevel == ctkDICOMJob::DICOMLevels::Instances)
  {
    return ctkDICOMJobResponseSet::JobType::QueryInstances;
  }
  else
  {
    return ctkDICOMJobResponseSet::JobType::None;
  }
}
