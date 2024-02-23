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
#include "ctkDICOMItem.h"
#include "ctkDICOMJobResponseSet.h"

// DCMTK includes
#include <dcmtk/dcmdata/dcdeftag.h>

static ctkLogger logger("org.commontk.dicom.DICOMJobResponseSet");

//------------------------------------------------------------------------------
class ctkDICOMJobResponseSetPrivate : public QObject
{
  Q_DECLARE_PUBLIC(ctkDICOMJobResponseSet);

protected:
  ctkDICOMJobResponseSet* const q_ptr;

public:
  ctkDICOMJobResponseSetPrivate(ctkDICOMJobResponseSet& obj);
  ~ctkDICOMJobResponseSetPrivate();

  QString FilePath;
  bool CopyFile;
  bool OverwriteExistingDataset;

  ctkDICOMJobResponseSet::JobType JobType;
  QString JobUID;
  QString PatientID;
  QString StudyInstanceUID;
  QString SeriesInstanceUID;
  QString SOPInstanceUID;
  QString ConnectionName;
  QMap<QString, QSharedPointer<ctkDICOMItem>> Datasets;
};

//------------------------------------------------------------------------------
// ctkDICOMJobResponseSetPrivate methods

//------------------------------------------------------------------------------
ctkDICOMJobResponseSetPrivate::ctkDICOMJobResponseSetPrivate(ctkDICOMJobResponseSet& obj)
  : q_ptr(&obj)
{
  this->JobType = ctkDICOMJobResponseSet::JobType::None;
  this->JobUID = "";
  this->PatientID = "";
  this->StudyInstanceUID = "";
  this->SeriesInstanceUID = "";
  this->SOPInstanceUID = "";
  this->ConnectionName = "";
  this->CopyFile = false;
  this->OverwriteExistingDataset = false;
  this->FilePath = "";
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSetPrivate::~ctkDICOMJobResponseSetPrivate()
{
  this->Datasets.clear();
}

//------------------------------------------------------------------------------
// ctkDICOMJobResponseSet methods

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::ctkDICOMJobResponseSet(QObject* parent)
  : QObject(parent),
    d_ptr(new ctkDICOMJobResponseSetPrivate(*this))
{
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::~ctkDICOMJobResponseSet() = default;

//----------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setFilePath(const QString& filePath)
{
  Q_D(ctkDICOMJobResponseSet);
  d->FilePath = filePath;

  if (d->FilePath.isEmpty())
  {
    return;
  }

  QSharedPointer<ctkDICOMItem> dataset =
    QSharedPointer<ctkDICOMItem>(new ctkDICOMItem);
  dataset->InitializeFromFile(filePath);

  DcmItem dcmItem = dataset->GetDcmItem();
  OFString SOPInstanceUID;
  dcmItem.findAndGetOFString(DCM_SOPInstanceUID, SOPInstanceUID);

  d->Datasets.insert(QString(SOPInstanceUID.c_str()), dataset);
}

//----------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::filePath() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->FilePath;
}

//----------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setCopyFile(bool copyFile)
{
  Q_D(ctkDICOMJobResponseSet);
  d->CopyFile = copyFile;
}

//----------------------------------------------------------------------------
bool ctkDICOMJobResponseSet::copyFile() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->CopyFile;
}

//----------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setOverwriteExistingDataset(bool overwriteExistingDataset)
{
  Q_D(ctkDICOMJobResponseSet);
  d->OverwriteExistingDataset = overwriteExistingDataset;
}

//----------------------------------------------------------------------------
bool ctkDICOMJobResponseSet::overwriteExistingDataset() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->OverwriteExistingDataset;
}

//----------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setJobType(ctkDICOMJobResponseSet::JobType jobType)
{
  Q_D(ctkDICOMJobResponseSet);
  d->JobType = jobType;
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet::JobType ctkDICOMJobResponseSet::jobType() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->JobType;
}

//------------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setJobUID(const QString& jobUID)
{
  Q_D(ctkDICOMJobResponseSet);
  d->JobUID = jobUID;
}

//------------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::jobUID() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->JobUID;
}

//------------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setPatientID(const QString& patientID)
{
  Q_D(ctkDICOMJobResponseSet);
  d->PatientID = patientID;
}

//------------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::patientID() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->PatientID;
}

//------------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setStudyInstanceUID(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMJobResponseSet);
  d->StudyInstanceUID = studyInstanceUID;
}

//------------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::studyInstanceUID() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->StudyInstanceUID;
}

//----------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setSeriesInstanceUID(const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMJobResponseSet);
  d->SeriesInstanceUID = seriesInstanceUID;
}

//------------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::seriesInstanceUID() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->SeriesInstanceUID;
}

//----------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setSOPInstanceUID(const QString& sopInstanceUID)
{
  Q_D(ctkDICOMJobResponseSet);
  d->SOPInstanceUID = sopInstanceUID;
}

//------------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::sopInstanceUID() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->SOPInstanceUID;
}

//------------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setConnectionName(const QString& connectionName)
{
  Q_D(ctkDICOMJobResponseSet);
  d->ConnectionName = connectionName;
}

//------------------------------------------------------------------------------
QString ctkDICOMJobResponseSet::connectionName() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->ConnectionName;
}

//------------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setDataset(DcmItem* dcmItem, bool takeOwnership)
{
  Q_D(ctkDICOMJobResponseSet);
  if (!dcmItem)
  {
    return;
  }

  QSharedPointer<ctkDICOMItem> dataset =
    QSharedPointer<ctkDICOMItem>(new ctkDICOMItem);
  dataset->InitializeFromItem(dcmItem, takeOwnership);

  OFString SOPInstanceUID;
  dcmItem->findAndGetOFString(DCM_SOPInstanceUID, SOPInstanceUID);
  d->Datasets.insert(QString(SOPInstanceUID.c_str()), dataset);
}

//------------------------------------------------------------------------------
ctkDICOMItem* ctkDICOMJobResponseSet::dataset() const
{
  Q_D(const ctkDICOMJobResponseSet);
  if (d->Datasets.count() == 0)
  {
    return nullptr;
  }
  return d->Datasets.first().data();
}

//------------------------------------------------------------------------------
QSharedPointer<ctkDICOMItem> ctkDICOMJobResponseSet::datasetShared() const
{
  Q_D(const ctkDICOMJobResponseSet);
  if (d->Datasets.count() == 0)
  {
    return nullptr;
  }
  return d->Datasets.first();
}

//------------------------------------------------------------------------------
void ctkDICOMJobResponseSet::setDatasets(const QMap<QString, DcmItem*>& dcmItems, bool takeOwnership)
{
  Q_D(ctkDICOMJobResponseSet);
  for (const QString& key : dcmItems.keys())
  {
    DcmItem* dcmItem = dcmItems.value(key);
    if (!dcmItem)
    {
      continue;
    }

    QSharedPointer<ctkDICOMItem> dataset =
      QSharedPointer<ctkDICOMItem>(new ctkDICOMItem);
    dataset->InitializeFromItem(dcmItem, takeOwnership);

    d->Datasets.insert(key, dataset);
  }
}

//------------------------------------------------------------------------------
QMap<QString, ctkDICOMItem*> ctkDICOMJobResponseSet::datasets() const
{
  Q_D(const ctkDICOMJobResponseSet);
  QMap<QString, ctkDICOMItem*> datasets;

  for (const QString& key : d->Datasets.keys())
  {
    QSharedPointer<ctkDICOMItem> dcmItem = d->Datasets.value(key);
    if (!dcmItem)
    {
      continue;
    }

    datasets.insert(key, dcmItem.data());
  }

  return datasets;
}

//------------------------------------------------------------------------------
QMap<QString, QSharedPointer<ctkDICOMItem>> ctkDICOMJobResponseSet::datasetsShared() const
{
  Q_D(const ctkDICOMJobResponseSet);
  return d->Datasets;
}

//------------------------------------------------------------------------------
ctkDICOMJobResponseSet* ctkDICOMJobResponseSet::clone()
{
  ctkDICOMJobResponseSet* newJobResponseSet = new ctkDICOMJobResponseSet;

  newJobResponseSet->setFilePath(this->filePath());
  newJobResponseSet->setCopyFile(this->copyFile());
  newJobResponseSet->setOverwriteExistingDataset(this->overwriteExistingDataset());
  newJobResponseSet->setJobType(this->jobType());
  newJobResponseSet->setJobUID(this->jobUID());
  newJobResponseSet->setPatientID(this->patientID());
  newJobResponseSet->setStudyInstanceUID(this->studyInstanceUID());
  newJobResponseSet->setSeriesInstanceUID(this->seriesInstanceUID());
  newJobResponseSet->setSOPInstanceUID(this->sopInstanceUID());
  newJobResponseSet->setConnectionName(this->connectionName());

  // Clone datasets
  QMap<QString, ctkDICOMItem*> datasets = this->datasets();
  for (const QString& key : datasets.keys())
  {
    ctkDICOMItem* dataset = datasets.value(key);
    if (!dataset)
    {
      continue;
    }
    QSharedPointer<ctkDICOMItem> newDataset =
      QSharedPointer<ctkDICOMItem>(dataset->Clone());
    newJobResponseSet->d_func()->Datasets.insert(key, newDataset);
  }

  return newJobResponseSet;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMJobResponseSet::toVariant()
{
  return QVariant::fromValue(ctkDICOMJobDetail(*this));
}
