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

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QDate>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QPair>
#include <QSet>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkDICOMQuery.h"
#include "ctkLogger.h"
#include "ctkDICOMJobResponseSet.h"

// DCMTK includes
#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/oflog/oflog.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/oflist.h>
#include <dcmtk/ofstd/ofstd.h>        /* for class OFStandard */
#include <dcmtk/dcmdata/dcddirif.h>   /* for class DicomDirInterface */

static ctkLogger logger ( "org.commontk.dicom.DICOMQuery" );

//------------------------------------------------------------------------------
// A customized implementation so that Qt signals can be emitted
// when query results are obtained
class ctkDICOMQuerySCUPrivate : public DcmSCU
{
public:
  ctkDICOMQuery *query;
  ctkDICOMQuerySCUPrivate()
  {
    this->query = 0;
  };
  ~ctkDICOMQuerySCUPrivate() {};
  virtual OFCondition handleFINDResponse(const T_ASC_PresentationContextID  presID,
                                         QRResponse *response,
                                         OFBool &waitForNextResponse)
  {
    if (!this->query)
    {
      return EC_IllegalCall;
    }

    if (this->query->wasCanceled())
    {
      // send cancel can fail and be ignored (but DCMTK will report still good == true).
      // Therefore, we need to force the release of the association to cancel the worker
      this->query->releaseAssociation();
      return EC_IllegalCall;
    }

    logger.debug ( "FIND RESPONSE" );
    emit this->query->debug(/*no tr*/"Got a find response!");
    return this->DcmSCU::handleFINDResponse(presID, response, waitForNextResponse);
  };
};

//------------------------------------------------------------------------------
class ctkDICOMQueryPrivate
{
public:
  ctkDICOMQueryPrivate();
  ~ctkDICOMQueryPrivate();

  /// Add a StudyInstanceUID to be queried
  void addStudyInstanceUIDAndDataset(const QString& studyInstanceUID, DcmDataset* dataset );
  /// Add StudyInstanceUID and SeriesInstanceUID that may be further retrieved
  void addStudyAndSeriesInstanceUID( const QString& studyInstanceUID, const QString& seriesInstanceUID );

  /// \warning: releaseAssociation is not a thread safe method.
  /// If called concurrently from different threads DCMTK can crash.
  /// Therefore use this method instead of calling directly SCU->releaseAssociation()
  OFCondition releaseAssociation();

  QString ConnectionName;
  QString CallingAETitle;
  QString CalledAETitle;
  QString Host;
  int Port;
  QMap<QString,QVariant> Filters;
  ctkDICOMQuerySCUPrivate *SCU;
  Uint16 PresentationContext;
  QSharedPointer<DcmDataset> QueryDcmDataset;
  QList<QPair<QString,QString>> StudyAndSeriesInstanceUIDPairList;
  QMap<QString, DcmDataset*> StudyDatasets;
  bool Canceled;
  bool AssociationClosing;
  QMutex AssociationMutex;
  int MaximumPatientsQuery;
  QString JobUID;
  QList<QSharedPointer<ctkDICOMJobResponseSet>> JobResponseSets;
};

//------------------------------------------------------------------------------
// ctkDICOMQueryPrivate methods

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::ctkDICOMQueryPrivate()
{
  this->QueryDcmDataset = QSharedPointer<DcmDataset>(new DcmDataset);
  this->Port = 0;
  this->Canceled = false;
  this->AssociationClosing = false;
  this->MaximumPatientsQuery = 25;

  this->PresentationContext = 0;
  this->SCU = new ctkDICOMQuerySCUPrivate();
  this->SCU->setACSETimeout(10);
  this->SCU->setConnectionTimeout(10);
}

//------------------------------------------------------------------------------
ctkDICOMQueryPrivate::~ctkDICOMQueryPrivate()
{
  if (this->SCU && this->SCU->isConnected())
  {
    this->releaseAssociation();
  }

  if (this->SCU)
  {
    delete this->SCU;
  }

  this->JobResponseSets.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMQueryPrivate::addStudyAndSeriesInstanceUID( const QString& studyInstanceUID, const QString& seriesInstanceUID )
{
  this->StudyAndSeriesInstanceUIDPairList.push_back (qMakePair( studyInstanceUID, seriesInstanceUID ) );
}

//------------------------------------------------------------------------------
void ctkDICOMQueryPrivate::addStudyInstanceUIDAndDataset( const QString& studyInstanceUID, DcmDataset* dataset )
{
  this->StudyDatasets[studyInstanceUID] = dataset;
}

//------------------------------------------------------------------------------
OFCondition ctkDICOMQueryPrivate::releaseAssociation()
{
  OFCondition status = EC_IllegalCall;
  if (!this->SCU)
  {
    return status;
  }

  QMutexLocker locker(&this->AssociationMutex);
  if (this->AssociationClosing)
  {
    return status;
  }

  this->AssociationClosing = true;
  status = this->SCU->releaseAssociation();
  this->AssociationClosing = false;

  return status;
}

//------------------------------------------------------------------------------
// ctkDICOMQuery methods

//------------------------------------------------------------------------------
ctkDICOMQuery::ctkDICOMQuery(QObject* parentObject)
  : QObject(parentObject)
  , d_ptr(new ctkDICOMQueryPrivate)
{
  Q_D(ctkDICOMQuery);

  d->SCU->setVerbosePCMode(false);
  d->SCU->query = this; // give the dcmtk level access to this for emitting signals
}

//------------------------------------------------------------------------------
ctkDICOMQuery::~ctkDICOMQuery()
{
}

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMQuery, const QString&, setConnectionName, ConnectionName);
CTK_GET_CPP(ctkDICOMQuery, QString, connectionName, ConnectionName)
CTK_SET_CPP(ctkDICOMQuery, const QString&, setCallingAETitle, CallingAETitle);
CTK_GET_CPP(ctkDICOMQuery, QString, callingAETitle, CallingAETitle)
CTK_SET_CPP(ctkDICOMQuery, const QString&, setCalledAETitle, CalledAETitle);
CTK_GET_CPP(ctkDICOMQuery, QString, calledAETitle, CalledAETitle)
CTK_SET_CPP(ctkDICOMQuery, const QString&, setHost, Host);
CTK_GET_CPP(ctkDICOMQuery, QString, host, Host)
CTK_SET_CPP(ctkDICOMQuery, const int&, setPort, Port);
CTK_GET_CPP(ctkDICOMQuery, int, port, Port)
CTK_SET_CPP(ctkDICOMQuery, const int&, setMaximumPatientsQuery, MaximumPatientsQuery);
CTK_GET_CPP(ctkDICOMQuery, int, maximumPatientsQuery, MaximumPatientsQuery);
CTK_SET_CPP(ctkDICOMQuery, const QString&, setJobUID, JobUID);
CTK_GET_CPP(ctkDICOMQuery, QString, jobUID, JobUID)

//-----------------------------------------------------------------------------
void ctkDICOMQuery::setConnectionTimeout(const int& timeout)
{
  Q_D(ctkDICOMQuery);
  d->SCU->setACSETimeout(timeout);
  d->SCU->setConnectionTimeout(timeout);
}

//-----------------------------------------------------------------------------
int ctkDICOMQuery::connectionTimeout() const
{
  Q_D(const ctkDICOMQuery);
  return d->SCU->getConnectionTimeout();
}

//------------------------------------------------------------------------------
bool ctkDICOMQuery::wasCanceled()
{
  Q_D(const ctkDICOMQuery);
  return d->Canceled;
}

//------------------------------------------------------------------------------
void ctkDICOMQuery::setFilters(const QMap<QString,QVariant>& filters)
{
  Q_D(ctkDICOMQuery);
  d->Filters = filters;
}

//------------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQuery::filters()const
{
  Q_D(const ctkDICOMQuery);
  return d->Filters;
}

//------------------------------------------------------------------------------
QList< QPair<QString,QString> > ctkDICOMQuery::studyAndSeriesInstanceUIDQueried()const
{
  Q_D(const ctkDICOMQuery);
  return d->StudyAndSeriesInstanceUIDPairList;
}

//------------------------------------------------------------------------------
QList<ctkDICOMJobResponseSet *> ctkDICOMQuery::jobResponseSets() const
{
  Q_D(const ctkDICOMQuery);
  QList<ctkDICOMJobResponseSet *> jobResponseSets;
  foreach(QSharedPointer<ctkDICOMJobResponseSet> jobResponseSet, d->JobResponseSets)
  {
    jobResponseSets.append(jobResponseSet.data());
  }

  return jobResponseSets;
}

//------------------------------------------------------------------------------
QList<QSharedPointer<ctkDICOMJobResponseSet>> ctkDICOMQuery::jobResponseSetsShared() const
{
  Q_D(const ctkDICOMQuery);
  return d->JobResponseSets;
}

//------------------------------------------------------------------------------
bool ctkDICOMQuery::query(ctkDICOMDatabase& database)
{
  Q_D(ctkDICOMQuery);
  // In the following, we emit progress(int) after progress(QString), this
  // is in case the connected object doesn't refresh its ui when the progress
  // message is updated but only if the progress value is (e.g. QProgressDialog)
  if (database.database().isOpen())
  {
    logger.debug("DB open in Query");
    emit progress(tr("DB open in Query"));
  }
  else
  {
    logger.warn("DB not open in Query");
    emit progress(tr("DB not open in Query"));
  }
  emit progress(0);
  if (d->Canceled)
  {
    return false;
  }

  d->StudyAndSeriesInstanceUIDPairList.clear();
  d->StudyDatasets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    return false;
  }

  // Clear the query
  d->QueryDcmDataset->clear();

  // Insert all keys that we like to receive values for
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientID);
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientName);
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientBirthDate);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyID);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyInstanceUID);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyDescription);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyDate);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyTime);
  d->QueryDcmDataset->insertEmptyElement(DCM_ModalitiesInStudy);
  d->QueryDcmDataset->insertEmptyElement(DCM_AccessionNumber);
  d->QueryDcmDataset->insertEmptyElement(DCM_NumberOfStudyRelatedInstances); // Number of images in the series
  d->QueryDcmDataset->insertEmptyElement(DCM_NumberOfStudyRelatedSeries); // Number of series in the study

  // Make clear we define our search values in ISO Latin 1 (default would be ASCII)
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SpecificCharacterSet, "ISO_IR 100");
  d->QueryDcmDataset->putAndInsertString (DCM_QueryRetrieveLevel, "STUDY");

  QString seriesDescription = this->applyFilters(d->Filters);
  if (d->Canceled)
  {
    return false;
  }

  OFList<QRResponse *> responses;

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    logger.error("Failed to find acceptable presentation context");
    emit progress(tr("Failed to find acceptable presentation context"));
  }
  else
  {
    logger.debug("Found useful presentation context");
    emit progress(tr("Found useful presentation context"));
  }
  emit progress(40);
  if (d->Canceled)
  {
    return false;
  }

  OFCondition status = d->SCU->sendFINDRequest(presentationContext, d->QueryDcmDataset.data(), &responses);
  if (!status.good())
  {
    logger.error("Find failed");
    emit progress(tr("Find failed"));
    d->releaseAssociation();
    emit progress(100);
    return false;
  }
  logger.debug("Find succeeded");
  emit progress(tr("Find succeeded"));
  emit progress(50);
  if (d->Canceled)
  {
    return false;
  }

  for (OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++)
  {
    DcmDataset *dataset = (*it)->m_dataset;
    if (dataset != NULL) // the last response is always empty
    {
      database.insert(dataset, false /* do not store to disk*/, false /* no thumbnail*/);
      OFString StudyInstanceUID;
      dataset->findAndGetOFString(DCM_StudyInstanceUID, StudyInstanceUID);
      d->addStudyInstanceUIDAndDataset(StudyInstanceUID.c_str(), dataset);
      emit progress(tr("Processing: ") + QString(StudyInstanceUID.c_str()));
      emit progress(50);
      if (d->Canceled)
      {
        return false;
      }
    }
  }

  /* Only ask for series attributes now. This requires kicking out the rest of former query. */
  d->QueryDcmDataset->clear();
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesNumber);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesDescription);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesInstanceUID);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesDate);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesTime);
  d->QueryDcmDataset->insertEmptyElement(DCM_Modality);
  d->QueryDcmDataset->insertEmptyElement(DCM_Rows);
  d->QueryDcmDataset->insertEmptyElement(DCM_Columns);
  d->QueryDcmDataset->insertEmptyElement(DCM_NumberOfSeriesRelatedInstances); // Number of images in the series

  /* Add user-defined filters */
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SeriesDescription, seriesDescription.toLatin1().data());

  // Now search each within each Study that was identified
  d->QueryDcmDataset->putAndInsertString(DCM_QueryRetrieveLevel, "SERIES");
  float progressRatio = 25. / d->StudyDatasets.count();
  int i = 0;

  foreach(QString studyInstanceUID, d->StudyDatasets.keys())
  {
    DcmDataset *studyDataset = d->StudyDatasets.value(studyInstanceUID);
    DcmElement *patientName, *patientID;
    studyDataset->findAndGetElement(DCM_PatientName, patientName);
    studyDataset->findAndGetElement(DCM_PatientID, patientID);

    logger.debug("Starting Series C-FIND for Study: " + studyInstanceUID);
    emit progress(tr("Starting Series C-FIND for Study: ") + studyInstanceUID);
    emit progress(50 + (progressRatio * i++));
    if (d->Canceled)
    {
    return false;
    }

    d->QueryDcmDataset->putAndInsertString (DCM_StudyInstanceUID, studyInstanceUID.toStdString().c_str());
    OFList<QRResponse *> responses;
    status = d->SCU->sendFINDRequest(presentationContext, d->QueryDcmDataset.data(), &responses);
    if (status.good())
    {
      for (OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++)
      {
        DcmDataset *dataset = (*it)->m_dataset;
        if (dataset != NULL)
        {
          OFString seriesInstanceUID;
          dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
          d->addStudyAndSeriesInstanceUID(studyInstanceUID.toStdString().c_str(), seriesInstanceUID.c_str());
          // add the patient elements not provided for the series level query
          dataset->insert(patientName, true);
          dataset->insert(patientID, true);
          // insert series dataset
          database.insert(dataset, false /* do not store */, false /* no thumbnail */);
        }
      }
      logger.debug ("Find succeeded on Series level for Study: " + studyInstanceUID);
      emit progress(tr("Find succeeded on Series level for Study: ") + studyInstanceUID);
      emit progress(50 + (progressRatio * i++));
      if (d->Canceled)
      {
    return false;
      }
    }
    else
    {
      logger.error ("Find on Series level failed for Study: " + studyInstanceUID);
      emit progress(tr("Find on Series level failed for Study: ") + studyInstanceUID);
    }
    emit progress(50 + (progressRatio * i++));
    if (d->Canceled)
    {
    return false;
    }
  }
  d->releaseAssociation();
  emit progress(100);
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::queryPatients()
{
  Q_D(ctkDICOMQuery);

  // In the following, we emit progress(int) after progress(QString), this
  // is in case the connected object doesn't refresh its ui when the progress
  // message is updated but only if the progress value is (e.g. QProgressDialog)
  emit progress(0);
  if (d->Canceled)
  {
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    return false;
  }

  // Clear the query
  d->QueryDcmDataset->clear();

  // Insert all keys that we like to receive values for
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientID);
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientName);
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientBirthDate);

  // Make clear we define our search values in ISO Latin 1 (default would be ASCII)
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SpecificCharacterSet, "ISO_IR 100");
  d->QueryDcmDataset->putAndInsertString(DCM_QueryRetrieveLevel, "PATIENT");

  QMap<QString,QVariant> filters;
  filters["Name"] = d->Filters["Name"];
  filters["ID"] = d->Filters["ID"];
  this->applyFilters(filters);
  if (d->Canceled)
  {
    return false;
  }

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    logger.error("Failed to find acceptable presentation context");
    emit progress(tr("Failed to find acceptable presentation context"));
  }
  else
  {
    logger.debug("Found useful presentation context");
    emit progress(tr("Found useful presentation context"));
  }
  emit progress(40);
  if (d->Canceled)
  {
    return false;
  }

  logger.debug("Starting Patients C-FIND");
  emit progress(tr("Starting Patients C-FIND"));
  emit progress(50);
  if (d->Canceled)
  {
    return false;
  }

  QSharedPointer<ctkDICOMJobResponseSet> JobResponseSet =
    QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
  JobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::QueryPatients);
  JobResponseSet->setConnectionName(d->ConnectionName);
  JobResponseSet->setJobUID(d->JobUID);

  QMap<QString, DcmItem*> datasetsMap;
  OFList<QRResponse *> responses;
  OFCondition status = d->SCU->sendFINDRequest(presentationContext, d->QueryDcmDataset.data(), &responses);
  if (status.good())
  {
    int contResponses = 0;
    for (OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++)
    {
      contResponses++;
      if (contResponses > d->MaximumPatientsQuery)
      {
        logger.warn(QString("ctkDICOMQuery: the number of responses of the query task at patients level "
                            "surpassed the maximum value of permitted results (i.e. %1).").arg(d->MaximumPatientsQuery));

        break;
      }
      DcmDataset *dataset = (*it)->m_dataset;
      if ( dataset != NULL )
      {
        OFString patientID;
        dataset->findAndGetOFString(DCM_PatientID, patientID);
        datasetsMap.insert(patientID.c_str(), dataset);
      }
    }

      JobResponseSet->setDatasets(datasetsMap);
      d->JobResponseSets.append(JobResponseSet);

      logger.debug("Find succeeded on Patient level");
      emit progress(tr("Find succeeded on Patient level"));
  }
  else
  {
    logger.error("Find on Patient level failed");
    emit progress(tr("Find on Patient level failed"));
  }

  emit progress(100);
  if (d->Canceled)
  {
    return false;
  }

  d->releaseAssociation();
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::queryStudies(const QString& patientID)
{
  Q_D(ctkDICOMQuery);
  // In the following, we emit progress(int) after progress(QString), this
  // is in case the connected object doesn't refresh its ui when the progress
  // message is updated but only if the progress value is (e.g. QProgressDialog)
  emit progress(0);
  if (d->Canceled)
  {
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    return false;
  }

  // Clear the query
  d->QueryDcmDataset->clear();

  // Insert all keys that we like to receive values for
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientID);
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientName);
  d->QueryDcmDataset->insertEmptyElement(DCM_PatientBirthDate);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyID);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyInstanceUID);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyDescription);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyDate);
  d->QueryDcmDataset->insertEmptyElement(DCM_StudyTime);
  d->QueryDcmDataset->insertEmptyElement(DCM_ModalitiesInStudy);
  d->QueryDcmDataset->insertEmptyElement(DCM_AccessionNumber);
  d->QueryDcmDataset->insertEmptyElement(DCM_NumberOfStudyRelatedSeries); // Number of series in the study

  // Make clear we define our search values in ISO Latin 1 (default would be ASCII)
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SpecificCharacterSet, "ISO_IR 100");
  d->QueryDcmDataset->putAndInsertString(DCM_QueryRetrieveLevel, "STUDY");

  QMap<QString,QVariant> filters;
  filters["Name"] = d->Filters["Name"];
  filters["ID"] = d->Filters["ID"];
  filters["Study"] = d->Filters["Study"];
  filters["AccessionNumber"] = d->Filters["AccessionNumber"];
  filters["Modalities"] = d->Filters["Modalities"];
  filters["StartDate"] = d->Filters["StartDate"];
  filters["EndDate"] = d->Filters["EndDate"];
  this->applyFilters(filters);
  if (d->Canceled)
  {
    return false;
  }

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    logger.error("Failed to find acceptable presentation context");
    emit progress(tr("Failed to find acceptable presentation context"));
  }
  else
  {
    logger.debug("Found useful presentation context");
    emit progress(tr("Found useful presentation context"));
  }
  emit progress(40);
  if (d->Canceled)
  {
    return false;
  }

  logger.debug("Starting Studies C-FIND for Patient: " + patientID);
  emit progress(tr("Starting Studies C-FIND for Patient: ") + patientID);
  emit progress(50);
  if (d->Canceled)
  {
    return false;
  }

  d->QueryDcmDataset->putAndInsertString(DCM_PatientID, patientID.toStdString().c_str());

  QSharedPointer<ctkDICOMJobResponseSet> JobResponseSet =
    QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
  JobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::QueryStudies);
  JobResponseSet->setPatientID(patientID.toStdString().c_str());
  JobResponseSet->setConnectionName(d->ConnectionName);
  JobResponseSet->setJobUID(d->JobUID);

  QMap<QString, DcmItem*> datasetsMap;

  OFList<QRResponse *> responses;
  OFCondition status = d->SCU->sendFINDRequest(presentationContext, d->QueryDcmDataset.data(), &responses);
  if (status.good())
  {
    for (OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++)
    {
      DcmDataset *dataset = (*it)->m_dataset;
      if ( dataset != NULL )
      {
        OFString studyInstanceUID;
        dataset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID);
        datasetsMap.insert(studyInstanceUID.c_str(), dataset);
      }
    }

    JobResponseSet->setDatasets(datasetsMap);
    d->JobResponseSets.append(JobResponseSet);

    logger.debug("Find succeeded on Study level for Patient: " + patientID);
    emit progress(tr("Find succeeded on Study level for Patient: ") + patientID);
  }
  else
  {
    logger.error("Find on Study level failed for Patient: " + patientID);
    emit progress(tr("Find on Study level failed for Patient: ") + patientID);
  }

  emit progress(100);
  if (d->Canceled)
  {
    return false;
  }

  d->releaseAssociation();
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::querySeries(const QString& patientID,
                                const QString& studyInstanceUID)
{
  Q_D(ctkDICOMQuery);

  // In the following, we emit progress(int) after progress(QString), this
  // is in case the connected object doesn't refresh its ui when the progress
  // message is updated but only if the progress value is (e.g. QProgressDialog)
  emit progress(0);
  if (d->Canceled)
  {
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    return false;
  }

  // Insert all keys that we like to receive values for
  d->QueryDcmDataset->clear();
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesNumber);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesDescription);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesInstanceUID);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesDate);
  d->QueryDcmDataset->insertEmptyElement(DCM_SeriesTime);
  d->QueryDcmDataset->insertEmptyElement(DCM_Modality);
  d->QueryDcmDataset->insertEmptyElement(DCM_NumberOfSeriesRelatedInstances); // Number of images in the series

  QMap<QString,QVariant> filters;
  filters["Name"] = d->Filters["Name"];
  filters["ID"] = d->Filters["ID"];
  filters["Study"] = d->Filters["Study"];
  filters["AccessionNumber"] = d->Filters["AccessionNumber"];
  filters["StartDate"] = d->Filters["StartDate"];
  filters["EndDate"] = d->Filters["EndDate"];
  filters["Series"] = d->Filters["Series"];
  QString seriesDescription = this->applyFilters(filters);
  if (d->Canceled)
  {
    return false;
  }

  /* Add user-defined filters */
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SeriesDescription, seriesDescription.toLatin1().data());
  d->QueryDcmDataset->putAndInsertString(DCM_QueryRetrieveLevel, "SERIES");

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    logger.error("Failed to find acceptable presentation context");
    emit progress(tr("Failed to find acceptable presentation context"));
  }
  else
  {
    logger.debug("Found useful presentation context");
    emit progress(tr("Found useful presentation context"));
  }
  emit progress(40);
  if (d->Canceled)
  {
    return false;
  }

  logger.debug("Starting Series C-FIND for Study: " + studyInstanceUID);
  emit progress(tr("Starting Series C-FIND for Study: ") + studyInstanceUID);
  emit progress(50);
  if (d->Canceled)
  {
    return false;
  }

  d->QueryDcmDataset->putAndInsertString(DCM_PatientID, patientID.toStdString().c_str());
  d->QueryDcmDataset->putAndInsertString(DCM_StudyInstanceUID, studyInstanceUID.toStdString().c_str());

  QSharedPointer<ctkDICOMJobResponseSet> JobResponseSet =
    QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
  JobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::QuerySeries);
  JobResponseSet->setPatientID(patientID.toStdString().c_str());
  JobResponseSet->setStudyInstanceUID(studyInstanceUID.toStdString().c_str());
  JobResponseSet->setConnectionName(d->ConnectionName);
  JobResponseSet->setJobUID(d->JobUID);

  QMap<QString, DcmItem*> datasetsMap;

  OFList<QRResponse *> responses;
  OFCondition status = d->SCU->sendFINDRequest(presentationContext, d->QueryDcmDataset.data(), &responses);
  if (status.good())
  {
    for (OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++)
    {
      DcmDataset *dataset = (*it)->m_dataset;
      if ( dataset != NULL )
      {
        OFString seriesInstanceUID;
        dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID);
        datasetsMap.insert(seriesInstanceUID.c_str(), dataset);
      }
    }

    JobResponseSet->setDatasets(datasetsMap);
    d->JobResponseSets.append(JobResponseSet);

    logger.debug("Find succeeded on Series level for Study: " + studyInstanceUID);
    emit progress(tr("Find succeeded on Series level for Study: ") + studyInstanceUID);
  }
  else
  {
    logger.error("Find on Series level failed for Study: " + studyInstanceUID);
    emit progress(tr("Find on Series level failed for Study: ") + studyInstanceUID);
  }

  emit progress(100);
  if (d->Canceled)
  {
    return false;
  }

  d->releaseAssociation();
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::queryInstances(const QString& patientID,
                                   const QString& studyInstanceUID,
                                   const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMQuery);

  // In the following, we emit progress(int) after progress(QString), this
  // is in case the connected object doesn't refresh its ui when the progress
  // message is updated but only if the progress value is (e.g. QProgressDialog)
  emit progress(0);
  if (d->Canceled)
  {
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    return false;
  }

  // Insert all keys that we like to receive values for
  d->QueryDcmDataset->clear();
  d->QueryDcmDataset->insertEmptyElement(DCM_InstanceNumber);
  d->QueryDcmDataset->insertEmptyElement(DCM_SOPInstanceUID);
  d->QueryDcmDataset->insertEmptyElement(DCM_Rows);
  d->QueryDcmDataset->insertEmptyElement(DCM_Columns);

  QMap<QString,QVariant> filters;
  filters["Name"] = d->Filters["Name"];
  filters["ID"] = d->Filters["ID"];
  filters["Study"] = d->Filters["Study"];
  filters["AccessionNumber"] = d->Filters["AccessionNumber"];
  filters["StartDate"] = d->Filters["StartDate"];
  filters["EndDate"] = d->Filters["EndDate"];
  filters["Series"] = d->Filters["Series"];
  QString seriesDescription = this->applyFilters(filters);
  if (d->Canceled)
  {
    return false;
  }

  /* Add user-defined filters */
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SeriesDescription, seriesDescription.toLatin1().data());
  d->QueryDcmDataset->putAndInsertString(DCM_QueryRetrieveLevel, "IMAGE");

  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  d->PresentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (d->PresentationContext == 0)
  {
    logger.error("Failed to find acceptable presentation context");
    emit progress(tr("Failed to find acceptable presentation context"));
  }
  else
  {
    logger.debug("Found useful presentation context");
    emit progress(tr("Found useful presentation context"));
  }
  emit progress(40);
  if (d->Canceled)
  {
    return false;
  }

  logger.debug("Starting Instances C-FIND for Series: " + seriesInstanceUID);
  emit progress(tr("Starting Instances C-FIND for Series: ") + seriesInstanceUID);
  emit progress(50);
  if (d->Canceled)
  {
    return false;
  }

  d->QueryDcmDataset->putAndInsertString(DCM_PatientID, patientID.toStdString().c_str());
  d->QueryDcmDataset->putAndInsertString(DCM_StudyInstanceUID, studyInstanceUID.toStdString().c_str());
  d->QueryDcmDataset->putAndInsertString(DCM_SeriesInstanceUID, seriesInstanceUID.toStdString().c_str());

  QSharedPointer<ctkDICOMJobResponseSet> JobResponseSet =
    QSharedPointer<ctkDICOMJobResponseSet>(new ctkDICOMJobResponseSet);
  JobResponseSet->setJobType(ctkDICOMJobResponseSet::JobType::QueryInstances);
  JobResponseSet->setPatientID(patientID.toStdString().c_str());
  JobResponseSet->setStudyInstanceUID(studyInstanceUID.toStdString().c_str());
  JobResponseSet->setSeriesInstanceUID(seriesInstanceUID.toStdString().c_str());
  JobResponseSet->setConnectionName(d->ConnectionName);
  JobResponseSet->setJobUID(d->JobUID);

  QMap<QString, DcmItem*> datasetsMap;

  OFList<QRResponse *> responses;
  OFCondition status = d->SCU->sendFINDRequest(d->PresentationContext, d->QueryDcmDataset.data(), &responses);
  if (status.good())
  {
    for (OFListIterator(QRResponse*) it = responses.begin(); it != responses.end(); it++)
    {
      DcmItem *dataset = (*it)->m_dataset;
      if (dataset != NULL)
      {
        OFString SOPInstanceUID;
        dataset->findAndGetOFString(DCM_SOPInstanceUID, SOPInstanceUID);
        datasetsMap.insert(SOPInstanceUID.c_str(), dataset);
      }
    }
    logger.debug("Find succeeded on Series level for Series: " + seriesInstanceUID);
    emit progress(tr("Find succeeded on Series level for Series: ") + seriesInstanceUID);
  }
  else
  {
    logger.error("Find on Series level failed for Series: " + seriesInstanceUID);
    emit progress(tr("Find on Series level failed for Series: ") + seriesInstanceUID);
  }

  JobResponseSet->setDatasets(datasetsMap);
  d->JobResponseSets.append(JobResponseSet);

  emit progress(100);
  if (d->Canceled)
  {
    return false;
  }

  d->releaseAssociation();
  return true;
}

//----------------------------------------------------------------------------
void ctkDICOMQuery::cancel()
{
  Q_D(ctkDICOMQuery);
  d->Canceled = true;

  if (d->PresentationContext != 0)
  {
    d->SCU->sendCANCELRequest(d->PresentationContext);
    d->PresentationContext = 0;
  }
}

//----------------------------------------------------------------------------
void ctkDICOMQuery::releaseAssociation()
{
  Q_D(ctkDICOMQuery);
  d->releaseAssociation();
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::initializeSCU()
{
  Q_D(ctkDICOMQuery);

  d->SCU->setAETitle(OFString(this->callingAETitle().toStdString().c_str()));
  d->SCU->setPeerAETitle(OFString(this->calledAETitle().toStdString().c_str()));
  d->SCU->setPeerHostName(OFString(this->host().toStdString().c_str()));
  d->SCU->setPeerPort(this->port());

  logger.debug("Setting Transfer Syntaxes");
  emit progress(tr("Setting Transfer Syntaxes"));
  emit progress(10);
  if (d->Canceled)
  {
    return false;
  }

  OFList<OFString> transferSyntaxes;
  transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
  transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
  transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

  d->SCU->addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes);
  if (!d->SCU->initNetwork().good())
  {
    logger.error("Error initializing the network");
    emit progress(tr("Error initializing the network"));
    emit progress(100);
    return false;
  }
  logger.debug("Negotiating Association");
  emit progress(tr("Negotiating Association"));
  emit progress(20);
  if (d->Canceled)
  {
    return false;
  }

  OFCondition result = d->SCU->negotiateAssociation();
  if (result.bad())
  {
    logger.error("Error negotiating the association: " + QString(result.text()));
    emit progress(tr("Error negotiating the association"));
    emit progress(100);
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
QString ctkDICOMQuery::applyFilters(QMap<QString,QVariant> filters)
{
  Q_D(ctkDICOMQuery);

  /* Now, for all keys that the user provided for filtering on STUDY level,
   * overwrite empty keys with value. For now, only Patient's Name, Patient ID,
   * Study Description, Modalities in Study, and Study Date are used.
   */
  QString seriesDescription;
  foreach(QString key, filters.keys())
  {
    if (key == QString("Name") && !filters[key].toString().isEmpty())
    {
      // make the filter a wildcard in dicom style
      d->QueryDcmDataset->putAndInsertString(DCM_PatientName,
        (QString("*") + filters[key].toString() + QString("*")).toLatin1().data());
    }
    else if (key == QString("ID") && !filters[key].toString().isEmpty())
    {
      // make the filter a wildcard in dicom style
      d->QueryDcmDataset->putAndInsertString(DCM_PatientID,
        (QString("*") + filters[key].toString() + QString("*")).toLatin1().data());
    }
    else if (key == QString("Study") && !filters[key].toString().isEmpty())
    {
      // make the filter a wildcard in dicom style
      d->QueryDcmDataset->putAndInsertString(DCM_StudyDescription,
        (QString("*") + filters[key].toString() + QString("*")).toLatin1().data());
    }
    else if (key == QString("AccessionNumber") && !filters[key].toString().isEmpty())
    {
      // make the filter a wildcard in dicom style
      d->QueryDcmDataset->putAndInsertString(DCM_AccessionNumber,
        (QString("*") + filters[key].toString() + QString("*")).toLatin1().data());
    }
    else if (key == QString("Modalities") && filters[key].toStringList().count() != 0)
    {
      // make the filter be an "OR" of modalities using backslash (dicom-style)
      QString modalitySearch("");
      foreach (const QString& modality, filters[key].toStringList())
      {
        modalitySearch += modality + QString("\\");
      }
      modalitySearch.chop(1); // remove final backslash
      logger.debug("modalityInStudySearch " + modalitySearch);
      d->QueryDcmDataset->putAndInsertString(DCM_ModalitiesInStudy, modalitySearch.toLatin1().data());
    }
    // Remember Series Description for later series query if we go through the keys now
    else if (key == QString("Series") && !filters[key].toString().isEmpty())
    {
      // make the filter a wildcard in dicom style
      seriesDescription = "*" + filters[key].toString() + "*";
    }
    else
    {
      logger.debug("Ignoring unknown search key: " + key);
    }
  }

  if (filters.keys().contains("StartDate") && filters.keys().contains("EndDate") &&
    !filters["StartDate"].toString().isEmpty() && !filters["EndDate"].toString().isEmpty())
  {
    QString dateRange = filters["StartDate"].toString() +
                        QString("-") +
                        filters["EndDate"].toString();
    d->QueryDcmDataset->putAndInsertString (DCM_StudyDate, dateRange.toLatin1().data());
    logger.debug("Query on study date " + dateRange);
  }

  emit progress(30);

  return seriesDescription;
}
