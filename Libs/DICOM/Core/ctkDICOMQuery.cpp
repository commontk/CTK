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

// ctkCore includes
#include <ctkPimpl.h>

// ctkDICOMCore includes
#include "ctkDICOMQuery.h"
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

//------------------------------------------------------------------------------
// Using dcmtk root log4cplus logger instead of ctkLogger because with ctkDICOMJobsAppender (dcmtk::log4cplus::Appender),
// logging is filtered by threadID and reported in the GUI per job.
dcmtk::log4cplus::Logger rootLogQuery = dcmtk::log4cplus::Logger::getRoot();

#define LOG_AND_EMIT_DEBUG(debugStr, signal) \
{ \
  DCMTK_LOG4CPLUS_DEBUG_STR(rootLogQuery, debugStr.toStdString().c_str()); \
  emit signal(debugStr); \
} \

#define LOG_AND_EMIT_WARN(warnStr, signal) \
{ \
  DCMTK_LOG4CPLUS_WARN_STR(rootLogQuery, warnStr.toStdString().c_str()); \
  emit signal(warnStr); \
} \

#define LOG_AND_EMIT_ERROR(errorStr, signal) \
{ \
  DCMTK_LOG4CPLUS_ERROR_STR(rootLogQuery, errorStr.toStdString().c_str()); \
  emit signal(errorStr); \
} \

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

    LOG_AND_EMIT_DEBUG(QString("FIND RESPONSE"), this->query->debug);
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
  if (database.database().isOpen())
  {
    LOG_AND_EMIT_DEBUG(QString("DB open in Query"), debug);
  }
  else
  {
    LOG_AND_EMIT_WARN(QString("DB not open in Query"), warn);
  }

  emit progress(0);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->StudyAndSeriesInstanceUIDPairList.clear();
  d->StudyDatasets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    emit done(false);
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
    emit done(false);
    return false;
  }

  OFList<QRResponse *> responses;
  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    LOG_AND_EMIT_ERROR(QString("Failed to find acceptable presentation context"), error);
  }
  else
  {
    LOG_AND_EMIT_DEBUG(QString("Found useful presentation context"), debug)
  }

  emit progress(40);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  OFCondition status = d->SCU->sendFINDRequest(presentationContext, d->QueryDcmDataset.data(), &responses);
  if (!status.good())
  {
    LOG_AND_EMIT_ERROR(QString("Find failed"), error);
    d->releaseAssociation();
    emit done(false);
    return false;
  }

  LOG_AND_EMIT_DEBUG(QString("Find succeeded"), debug)

  emit progress(50);
  if (d->Canceled)
  {
    emit done(false);
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
      emit progress("Processing: " + QString(StudyInstanceUID.c_str()));
      emit progress(50);
      if (d->Canceled)
      {
        emit done(false);
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

    LOG_AND_EMIT_DEBUG(QString("Starting Series C-FIND for Study: %1").arg(studyInstanceUID), debug)
    emit progress(50 + (progressRatio * i++));
    if (d->Canceled)
    {
      emit done(false);
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

      LOG_AND_EMIT_DEBUG(QString("Find succeeded at Series level for Study: %1").arg(studyInstanceUID), debug)
      emit progress(50 + (progressRatio * i++));
      if (d->Canceled)
      {
        emit done(false);
        return false;
      }
    }
    else
    {
      LOG_AND_EMIT_ERROR(QString("Find at Series level failed for Study: %1").arg(studyInstanceUID), error)
    }
    emit progress(50 + (progressRatio * i++));
    if (d->Canceled)
    {
    emit done(false);
    return false;
    }
  }
  d->releaseAssociation();
  emit progress(100);
  emit done(true);
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::queryPatients()
{
  Q_D(ctkDICOMQuery);

  emit progress(0);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    emit done(false);
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
    emit done(false);
    return false;
  }

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    LOG_AND_EMIT_ERROR(QString("Failed to find acceptable presentation context"), error)
  }
  else
  {
    LOG_AND_EMIT_DEBUG(QString("Found useful presentation context"), debug)
  }
  emit progress(40);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  LOG_AND_EMIT_DEBUG(QString("Starting patients C-FIND"), debug)
  emit progress(50);
  if (d->Canceled)
  {
    emit done(false);
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
        LOG_AND_EMIT_WARN(QString("The number of responses of the query task at patients level "
                                  "surpassed the maximum value of permitted results (i.e. %1).").arg(d->MaximumPatientsQuery), warn)
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

    if (contResponses == 0)
    {
      LOG_AND_EMIT_WARN(QString("The patients query provided no results. Please refine your filters."), warn)
    }

    JobResponseSet->setDatasets(datasetsMap);
    d->JobResponseSets.append(JobResponseSet);

    LOG_AND_EMIT_DEBUG(QString("Find succeeded at patient level"), debug)
  }
  else
  {
    LOG_AND_EMIT_ERROR(QString("Find at patient level failed"), error)
  }

  emit progress(100);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->releaseAssociation();
  emit done(true);
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::queryStudies(const QString& patientID)
{
  Q_D(ctkDICOMQuery);

  emit progress(0);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    emit done(false);
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
    emit done(false);
    return false;
  }

  Uint16 presentationContext = 0;
  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  presentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (presentationContext == 0)
  {
    LOG_AND_EMIT_ERROR(QString("Failed to find acceptable presentation context"), error);
  }
  else
  {
    LOG_AND_EMIT_DEBUG(QString("Found useful presentation context"), debug);
  }
  emit progress(40);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  LOG_AND_EMIT_DEBUG(QString("Starting studies C-FIND for patient: %1").arg(patientID), debug)
  emit progress(50);
  if (d->Canceled)
  {
    emit done(false);
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

    LOG_AND_EMIT_DEBUG(QString("Find succeeded at study level for patient: %1").arg(patientID), debug)
  }
  else
  {
    LOG_AND_EMIT_ERROR(QString("Find at study level failed for patient: %1").arg(patientID), error)
  }

  emit progress(100);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->releaseAssociation();
  emit done(true);
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::querySeries(const QString& patientID,
                                const QString& studyInstanceUID)
{
  Q_D(ctkDICOMQuery);

  emit progress(0);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    emit done(false);
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
    emit done(false);
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
    LOG_AND_EMIT_ERROR(QString("Failed to find acceptable presentation context"), error);
  }
  else
  {
    LOG_AND_EMIT_DEBUG(QString("Found useful presentation context"), debug);
  }
  emit progress(40);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  LOG_AND_EMIT_DEBUG(QString("Starting series C-FIND for study: %1").arg(studyInstanceUID), debug)
  emit progress(50);
  if (d->Canceled)
  {
    emit done(false);
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

    LOG_AND_EMIT_DEBUG(QString("Find succeeded at series level for study: %1").arg(studyInstanceUID), debug)
  }
  else
  {
    LOG_AND_EMIT_ERROR(QString("Find at series level failed for study: %1").arg(studyInstanceUID), error)
  }

  emit progress(100);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->releaseAssociation();
  emit done(true);
  return true;
}

//----------------------------------------------------------------------------
bool ctkDICOMQuery::queryInstances(const QString& patientID,
                                   const QString& studyInstanceUID,
                                   const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMQuery);

  emit progress(0);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->JobResponseSets.clear();

  // initSCU
  if (!this->initializeSCU())
  {
    emit done(false);
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
    emit done(false);
    return false;
  }

  /* Add user-defined filters */
  d->QueryDcmDataset->putAndInsertOFStringArray(DCM_SeriesDescription, seriesDescription.toLatin1().data());
  d->QueryDcmDataset->putAndInsertString(DCM_QueryRetrieveLevel, "IMAGE");

  // Check for any accepted presentation context for FIND in study root (don't care about transfer syntax)
  d->PresentationContext = d->SCU->findPresentationContextID(UID_FINDStudyRootQueryRetrieveInformationModel, "");
  if (d->PresentationContext == 0)
  {
    LOG_AND_EMIT_ERROR(QString("Failed to find acceptable presentation context"), error)
  }
  else
  {
    LOG_AND_EMIT_DEBUG(QString("Found useful presentation context"), debug)
  }
  emit progress(40);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  LOG_AND_EMIT_DEBUG(QString("Starting sop instances C-FIND for series: %1").arg(seriesInstanceUID), debug)
  emit progress(50);
  if (d->Canceled)
  {
    emit done(false);
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

    LOG_AND_EMIT_DEBUG(QString("Find succeeded at sop instance level for series: %1").arg(seriesInstanceUID), debug)
  }
  else
  {
    LOG_AND_EMIT_ERROR(QString("Find at sop instance level failed for series: %1").arg(seriesInstanceUID), error)
  }

  JobResponseSet->setDatasets(datasetsMap);
  d->JobResponseSets.append(JobResponseSet);

  emit progress(100);
  if (d->Canceled)
  {
    emit done(false);
    return false;
  }

  d->releaseAssociation();
  emit done(true);
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

  LOG_AND_EMIT_DEBUG(QString("Setting Transfer Syntaxes"), debug)
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
    LOG_AND_EMIT_ERROR(QString("Error initializing the network"), error)
    return false;
  }

  LOG_AND_EMIT_DEBUG(QString("Negotiating Association"), debug)
  emit progress(20);
  if (d->Canceled)
  {
    return false;
  }

  OFCondition result = d->SCU->negotiateAssociation();
  if (result.bad())
  {
    LOG_AND_EMIT_ERROR(QString("Error negotiating the association: %1").arg(result.text()), error)
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

      LOG_AND_EMIT_DEBUG(QString("modalityInStudySearch %1").arg(modalitySearch), debug)
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
      LOG_AND_EMIT_DEBUG(QString("Ignoring unknown search key: %1").arg(key), debug)
    }
  }

  if (filters.keys().contains("StartDate") && filters.keys().contains("EndDate") &&
    !filters["StartDate"].toString().isEmpty() && !filters["EndDate"].toString().isEmpty())
  {
    QString dateRange = filters["StartDate"].toString() +
                        QString("-") +
                        filters["EndDate"].toString();
    d->QueryDcmDataset->putAndInsertString (DCM_StudyDate, dateRange.toLatin1().data());

    LOG_AND_EMIT_DEBUG(QString("Query on study date: %1").arg(dateRange), debug)
  }

  emit progress(30);

  return seriesDescription;
}
