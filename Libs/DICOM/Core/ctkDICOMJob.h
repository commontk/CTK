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

#ifndef __ctkDICOMJob_h
#define __ctkDICOMJob_h

// Qt includes
#include <QObject>
#include <QSharedPointer>
#include <QVariant>

// ctkCore includes
#include <ctkAbstractJob.h>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMJobResponseSet.h"

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMJob : public ctkAbstractJob
{
  Q_OBJECT
  Q_PROPERTY(QString patientID READ patientID WRITE setPatientID);
  Q_PROPERTY(QString studyInstanceUID READ studyInstanceUID WRITE setStudyInstanceUID);
  Q_PROPERTY(QString seriesInstanceUID READ seriesInstanceUID WRITE setSeriesInstanceUID);
  Q_PROPERTY(QString sopInstanceUID READ sopInstanceUID WRITE setSOPInstanceUID);
  Q_PROPERTY(DICOMLevels dicomLevel READ dicomLevel WRITE setDICOMLevel);
  Q_PROPERTY(QString referenceInserterJobUID READ referenceInserterJobUID WRITE setReferenceInserterJobUID);

public:
  typedef ctkAbstractJob Superclass;
  explicit ctkDICOMJob();
  virtual ~ctkDICOMJob();

  enum DICOMLevels
  {
    None,
    Patients,
    Studies,
    Series,
    Instances
  };
  Q_ENUM(DICOMLevels)

  ///@{
  /// DICOM Level
  void setDICOMLevel(const DICOMLevels& dicomLevel);
  DICOMLevels dicomLevel() const;
  ///@}

  ///@{
  /// Patient ID
  void setPatientID(const QString& patientID);
  QString patientID() const;
  ///@}

  ///@{
  /// Study instance UID
  void setStudyInstanceUID(const QString& studyInstanceUID);
  QString studyInstanceUID() const;
  ///@}

  ///@{
  /// Series instance UID
  void setSeriesInstanceUID(const QString& seriesInstanceUID);
  QString seriesInstanceUID() const;
  ///@}

  ///@{
  /// SOP instance UID
  void setSOPInstanceUID(const QString& sopInstanceUID);
  QString sopInstanceUID() const;
  ///@}

  ///@{
  /// job UID of the inserter for this job
  void setReferenceInserterJobUID(const QString& referenceInserterJobUID);
  QString referenceInserterJobUID() const;
  ///@}

  ///@{
  /// Access the list of responses.
  Q_INVOKABLE QList<ctkDICOMJobResponseSet*> jobResponseSets() const;
  QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSetsShared() const;
  Q_INVOKABLE void setJobResponseSets(const QList<ctkDICOMJobResponseSet*>& jobResponseSets);
  void setJobResponseSets(const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets);
  void copyJobResponseSets(const QList<QSharedPointer<ctkDICOMJobResponseSet>>& jobResponseSets);
  ///@}

  /// Return job type.
  Q_INVOKABLE virtual ctkDICOMJobResponseSet::JobType getJobType() const;

  /// Return the QVariant value of this job.
  ///
  /// The value is set using the ctkDICOMJobDetail metatype and is used to pass
  /// information between threads using Qt signals.
  /// \sa ctkDICOMJobDetail
  Q_INVOKABLE virtual QVariant toVariant() override;

  /// Free used resources from job
  /// \sa ctkAbstractJob::releaseResources
  Q_INVOKABLE virtual void releaseResources() override;

Q_SIGNALS:
  void progressJobDetail(QVariant);

protected:
  QString PatientID;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString SOPInstanceUID;
  QString ReferenceInserterJobUID;
  ctkDICOMJob::DICOMLevels DICOMLevel;
  QList<QSharedPointer<ctkDICOMJobResponseSet>> JobResponseSets;

private:
  Q_DISABLE_COPY(ctkDICOMJob);
};

//------------------------------------------------------------------------------
struct CTK_DICOM_CORE_EXPORT ctkDICOMJobDetail : ctkJobDetail
{
  explicit ctkDICOMJobDetail() = default;

  explicit ctkDICOMJobDetail(const ctkDICOMJob& job) : ctkJobDetail(job)
  {
    this->DICOMLevel = job.dicomLevel();
    this->JobType = job.getJobType();
    this->PatientID = job.patientID();
    this->StudyInstanceUID = job.studyInstanceUID();
    this->SeriesInstanceUID = job.seriesInstanceUID();
    this->SOPInstanceUID = job.sopInstanceUID();
    this->ReferenceInserterJobUID = job.referenceInserterJobUID();
  }

  explicit ctkDICOMJobDetail(const ctkDICOMJob& job, const QString& connectionName)
    : ctkDICOMJobDetail(job)
  {
    this->ConnectionName = connectionName;
  }

  explicit ctkDICOMJobDetail(const ctkDICOMJobResponseSet& responseSet)
  {
    this->JobUID = responseSet.jobUID();
    this->JobType = responseSet.jobType();
    this->PatientID = responseSet.patientID();
    this->StudyInstanceUID = responseSet.studyInstanceUID();
    this->SeriesInstanceUID = responseSet.seriesInstanceUID();
    this->SOPInstanceUID = responseSet.sopInstanceUID();
    this->ConnectionName = responseSet.connectionName();
    this->NumberOfDataSets = responseSet.datasets().count();
    if (this->JobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      this->QueriedPatientIDs = responseSet.datasets().keys();
    }
    else if (this->JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
    {
      this->QueriedStudyInstanceUIDs = responseSet.datasets().keys();
    }
    else if (this->JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
    {
      this->QueriedSeriesInstanceUIDs = responseSet.datasets().keys();
    }
    else if (this->JobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
    {
      this->QueriedSOPInstanceUIDs = responseSet.datasets().keys();
    }
  }
  virtual ~ctkDICOMJobDetail() = default;

  QString PatientID;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString SOPInstanceUID;
  QString ReferenceInserterJobUID;
  QStringList QueriedPatientIDs;
  QStringList QueriedStudyInstanceUIDs;
  QStringList QueriedSeriesInstanceUIDs;
  QStringList QueriedSOPInstanceUIDs;

  // Common to DICOM Query and Retrieve jobs, and DICOM JobResponseSet
  QString ConnectionName;

  // Specific to DICOM Query and Retrieve jobs
  ctkDICOMJob::DICOMLevels DICOMLevel{ctkDICOMJob::DICOMLevels::None};

  // Specific to DICOM JobResponseSet
  ctkDICOMJobResponseSet::JobType JobType{ctkDICOMJobResponseSet::JobType::None};
  int NumberOfDataSets{0};
};
Q_DECLARE_METATYPE(ctkDICOMJobDetail);

#endif
