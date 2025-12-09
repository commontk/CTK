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

#ifndef __ctkDICOMJobResponseSet_h
#define __ctkDICOMJobResponseSet_h

// Qt includes
#include <QObject>
#include <QMap>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMItem.h"

class ctkDICOMJobResponseSetPrivate;

/// \ingroup DICOM_Core
class CTK_DICOM_CORE_EXPORT ctkDICOMJobResponseSet : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged);
  Q_PROPERTY(bool copyFile READ copyFile WRITE setCopyFile NOTIFY copyFileChanged);
  Q_PROPERTY(bool overwriteExistingDataset READ overwriteExistingDataset WRITE setOverwriteExistingDataset NOTIFY overwriteExistingDatasetChanged);
  Q_PROPERTY(JobType jobType READ jobType WRITE setJobType NOTIFY jobTypeChanged);
  Q_PROPERTY(QString jobUID READ jobUID WRITE setJobUID NOTIFY jobUIDChanged);
  Q_PROPERTY(QString patientID READ patientID WRITE setPatientID  NOTIFY patientIDChanged);
  Q_PROPERTY(QString studyInstanceUID READ studyInstanceUID WRITE setStudyInstanceUID NOTIFY studyInstanceUIDChanged);
  Q_PROPERTY(QString seriesInstanceUID READ seriesInstanceUID WRITE setSeriesInstanceUID NOTIFY seriesInstanceUIDChanged);
  Q_PROPERTY(QString sopInstanceUID READ sopInstanceUID WRITE setSOPInstanceUID NOTIFY sopInstanceUIDChanged);
  Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged);

public:
  explicit ctkDICOMJobResponseSet(QObject* parent = 0);
  virtual ~ctkDICOMJobResponseSet();

  ///@{
  /// File Path
  void setFilePath(const QString& filePath);
  QString filePath() const;
  ///@}

  ///@{
  /// Copy File
  /// false as default
  void setCopyFile(bool copyFile);
  bool copyFile() const;
  ///@}

  ///@{
  /// Overwrite existing dataset
  /// false as default
  void setOverwriteExistingDataset(bool overwriteExistingDataset);
  bool overwriteExistingDataset() const;
  ///@}

  ///@{
  /// Job type
  enum JobType
  {
    None = 0,
    QueryPatients,
    QueryStudies,
    QuerySeries,
    QueryInstances,
    RetrieveStudy,
    RetrieveSeries,
    RetrieveSOPInstance,
    StoreSOPInstance,
    Inserter,
    Echo,
    ThumbnailGenerator,
  };
  Q_ENUM(JobType)
  void setJobType(JobType jobType);
  JobType jobType() const;
  QString jobTypeString() const;
  ///@}

  ///@{
  /// Job UID
  void setJobUID(const QString& jobUID);
  QString jobUID() const;
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
  /// Connection name
  void setConnectionName(const QString& connectionName);
  QString connectionName() const;
  ///@}

  ///@{
  /// DCM datasets
  Q_INVOKABLE void setDataset(DcmItem* dcmItem, bool takeOwnership = true);
  Q_INVOKABLE ctkDICOMItem* dataset() const;
  QSharedPointer<ctkDICOMItem> datasetShared() const;
  Q_INVOKABLE void setDatasets(const QMap<QString, DcmItem*>& dcmItems, bool takeOwnership = true);
  Q_INVOKABLE QMap<QString, ctkDICOMItem*> datasets() const;
  QMap<QString, QSharedPointer<ctkDICOMItem>> datasetsShared() const;
  ///@}

  /// Create a copy of this JobResponseSet.
  Q_INVOKABLE ctkDICOMJobResponseSet* clone();

  /// Return the QVariant value of this JobResponseSet.
  ///
  /// The value is set using the ctkDICOMJobDetail metatype and is used to pass
  /// information between threads using Qt signals.
  Q_INVOKABLE QVariant toVariant();

Q_SIGNALS:
  void filePathChanged(const QString& filePath);
  void copyFileChanged(bool copyFile);
  void overwriteExistingDatasetChanged(bool overwriteExistingDataset);
  void jobTypeChanged(const ctkDICOMJobResponseSet::JobType & jobType);
  void jobUIDChanged(const QString& jobUID);
  void patientIDChanged(const QString& patientID);
  void studyInstanceUIDChanged(const QString& studyInstanceUID);
  void seriesInstanceUIDChanged(const QString& seriesInstanceUID);
  void sopInstanceUIDChanged(const QString& sopInstanceUID);
  void connectionNameChanged(const QString& connectionName);


protected:
  QScopedPointer<ctkDICOMJobResponseSetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMJobResponseSet);
  Q_DISABLE_COPY(ctkDICOMJobResponseSet);
};

#endif
