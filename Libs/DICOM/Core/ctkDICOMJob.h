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

#ifndef __ctkDICOMJob_h
#define __ctkDICOMJob_h

// Qt includes
#include <QObject>
#include <QSharedPointer>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMWorker.h"

#include <ctkAbstractJob.h>

class ctkDICOMServer;
class ctkDICOMJobResponseSet;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMJob : public ctkAbstractJob
{
  Q_OBJECT
  Q_ENUMS(DICOMLevel)
  Q_PROPERTY(QString studyInstanceUID READ studyInstanceUID WRITE setStudyInstanceUID);
  Q_PROPERTY(QString seriesInstanceUID READ seriesInstanceUID WRITE setSeriesInstanceUID);
  Q_PROPERTY(QString sopInstanceUID READ sopInstanceUID WRITE setSOPInstanceUID);
  Q_PROPERTY(DICOMLevels dicomLevel READ dicomLevel WRITE setDICOMLevel);

public:
  typedef ctkAbstractJob Superclass;
  explicit ctkDICOMJob();
  virtual ~ctkDICOMJob();

  enum DICOMLevels{
    Patients,
    Studies,
    Series,
    Instances
  };

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
  /// Access the list of responses.
  Q_INVOKABLE QList<ctkDICOMJobResponseSet*> jobResponseSets() const;
  QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSetsShared() const;
  Q_INVOKABLE void setJobResponseSets(QList<ctkDICOMJobResponseSet*> jobResponseSets);
  void setJobResponseSets(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets);
  void copyJobResponseSets(QList<QSharedPointer<ctkDICOMJobResponseSet>> jobResponseSets);
  ///@}

  /// Create a copy of the object
  Q_INVOKABLE virtual ctkDICOMJob* generateCopy() const = 0;

Q_SIGNALS:
  void progressJobDetail(QVariant);
  void finishedJobDetail(QVariant);

protected:
  QString PatientID;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString SOPInstanceUID;
  ctkDICOMJob::DICOMLevels DICOMLevel;
  QList<QSharedPointer<ctkDICOMJobResponseSet>> JobResponseSets;

private:
  Q_DISABLE_COPY(ctkDICOMJob);
};

#endif
