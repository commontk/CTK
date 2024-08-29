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
#include "ctkDICOMThumbnailGeneratorJob_p.h"
#include "ctkDICOMThumbnailGeneratorWorker.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMThumbnailGeneratorJob" );

//------------------------------------------------------------------------------
// ctkDICOMThumbnailGeneratorJobPrivate methods

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorJobPrivate::ctkDICOMThumbnailGeneratorJobPrivate(ctkDICOMThumbnailGeneratorJob* object)
 : q_ptr(object)
{
  this->DatabaseFilename = "";
  this->DicomFilePath = "";
  this->Modality = "";
  this->BackgroundColor = Qt::darkGray;
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorJobPrivate::~ctkDICOMThumbnailGeneratorJobPrivate()
{
}

//------------------------------------------------------------------------------
CTK_GET_CPP(ctkDICOMThumbnailGeneratorJob, QString, databaseFilename, DatabaseFilename);
CTK_SET_CPP(ctkDICOMThumbnailGeneratorJob, QString, setDatabaseFilename, DatabaseFilename);
CTK_GET_CPP(ctkDICOMThumbnailGeneratorJob, QString, dicomFilePath, DicomFilePath);
CTK_SET_CPP(ctkDICOMThumbnailGeneratorJob, QString, setDicomFilePath, DicomFilePath);
CTK_GET_CPP(ctkDICOMThumbnailGeneratorJob, QString, modality, Modality);
CTK_SET_CPP(ctkDICOMThumbnailGeneratorJob, QString, setModality, Modality);
CTK_GET_CPP(ctkDICOMThumbnailGeneratorJob, QColor, backgroundColor, BackgroundColor);
CTK_SET_CPP(ctkDICOMThumbnailGeneratorJob, QColor, setBackgroundColor, BackgroundColor);

//------------------------------------------------------------------------------
// ctkDICOMThumbnailGeneratorJob methods

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorJob::ctkDICOMThumbnailGeneratorJob()
  : d_ptr(new ctkDICOMThumbnailGeneratorJobPrivate(this))
{
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorJob::ctkDICOMThumbnailGeneratorJob(ctkDICOMThumbnailGeneratorJobPrivate* pimpl)
  : d_ptr(pimpl)
{
}

//------------------------------------------------------------------------------
ctkDICOMThumbnailGeneratorJob::~ctkDICOMThumbnailGeneratorJob() = default;

//----------------------------------------------------------------------------
QString ctkDICOMThumbnailGeneratorJob::loggerReport(const QString& status)
{
  QString fullLogMsg = QString("ctkDICOMThumbnailGeneratorJob: thumbnail generator job %1.\n")
                          .arg(status);
  QString logMsg = QString("Thumbnail generator job %1.\n")
                          .arg(status);
  QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
  QString logHeader = currentDateTime + " INFO: ";
  this->LoggedText += logHeader;
  this->LoggedText += logMsg;
  return fullLogMsg;
}
//------------------------------------------------------------------------------
ctkAbstractJob* ctkDICOMThumbnailGeneratorJob::clone() const
{
  ctkDICOMThumbnailGeneratorJob* newThumbnailGeneratorJob = new ctkDICOMThumbnailGeneratorJob;
  newThumbnailGeneratorJob->setMaximumNumberOfRetry(this->maximumNumberOfRetry());
  newThumbnailGeneratorJob->setRetryDelay(this->retryDelay());
  newThumbnailGeneratorJob->setRetryCounter(this->retryCounter());
  newThumbnailGeneratorJob->setIsPersistent(this->isPersistent());
  newThumbnailGeneratorJob->setMaximumConcurrentJobsPerType(this->maximumConcurrentJobsPerType());
  newThumbnailGeneratorJob->setPriority(this->priority());
  newThumbnailGeneratorJob->setBackgroundColor(this->backgroundColor());
  newThumbnailGeneratorJob->setModality(this->modality());
  newThumbnailGeneratorJob->setDicomFilePath(this->dicomFilePath());

  return newThumbnailGeneratorJob;
}

//------------------------------------------------------------------------------
ctkAbstractWorker* ctkDICOMThumbnailGeneratorJob::createWorker()
{
  ctkDICOMThumbnailGeneratorWorker* worker =
    new ctkDICOMThumbnailGeneratorWorker;
  worker->setJob(*this);
  return worker;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMThumbnailGeneratorJob::toVariant()
{
  return QVariant::fromValue(ctkDICOMJobDetail(*this));
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::JobType ctkDICOMThumbnailGeneratorJob::getJobType() const
{
  return ctkDICOMJobResponseSet::JobType::ThumbnailGenerator;
}
