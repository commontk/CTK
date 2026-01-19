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

#ifndef __ctkDICOMCorePythonQtDecorators_h
#define __ctkDICOMCorePythonQtDecorators_h

// PythonQt includes
#include <PythonQt.h>

// CTK Core includes
#include <ctkLogger.h>

// CTK includes
#include <ctkDICOMDisplayedFieldGeneratorRuleFactory.h>
#include <ctkDICOMUtil.h>
#include <ctkDICOMJob.h>
#include <ctkDICOMJobResponseSet.h>

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

static ctkLogger logger("org.commontk.core.ctkDICOMCorePythonQtDecorators");

/// \ingroup DICOM_Core
class ctkDICOMCorePythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkDICOMCorePythonQtDecorators()
  {
    PythonQt::self()->registerCPPClass("ctkDICOMJobDetail", 0, "CTKDICOMCore");
    PythonQt::self()->addParentClass("ctkDICOMJobDetail", "ctkJobDetail",
                                     PythonQtUpcastingOffset<ctkDICOMJobDetail,ctkJobDetail>());
  }

public slots:

  //----------------------------------------------------------------------------
  // ctkDICOMJobDetail
  //----------------------------------------------------------------------------
  ctkDICOMJobDetail* new_ctkDICOMJobDetail()
  {
    return new ctkDICOMJobDetail();
  }

  void setPatientID(ctkDICOMJobDetail* td, const QString& patientID)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setPatientID - Invalid ctkJobDetail");
      return;
    }

    td->PatientID = patientID;
  }
  QString patientID(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::patientID - Invalid ctkJobDetail");
      return "";
    }

    return td->PatientID;
  }

  void setStudyInstanceUID(ctkDICOMJobDetail* td, const QString& studyInstanceUID)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setStudyInstanceUID - Invalid ctkJobDetail");
      return;
    }

    td->StudyInstanceUID = studyInstanceUID;
  }
  QString studyInstanceUID(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::studyInstanceUID - Invalid ctkJobDetail");
      return "";
    }

    return td->StudyInstanceUID;
  }

  void setSeriesInstanceUID(ctkDICOMJobDetail* td, const QString& seriesInstanceUID)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setSeriesInstanceUID - Invalid ctkJobDetail");
      return;
    }

    td->SeriesInstanceUID = seriesInstanceUID;
  }
  QString seriesInstanceUID(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::seriesInstanceUID - Invalid ctkJobDetail");
      return "";
    }

    return td->SeriesInstanceUID;
  }

  void setSOPInstanceUID(ctkDICOMJobDetail* td, const QString& sopInstanceUID)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setSOPInstanceUID - Invalid ctkJobDetail");
      return;
    }

    td->SOPInstanceUID = sopInstanceUID;
  }
  QString sopInstanceUID(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::sopInstanceUID - Invalid ctkJobDetail");
      return "";
    }

    return td->SOPInstanceUID;
  }

  void setReferenceInserterJobUID(ctkDICOMJobDetail* td, const QString& referenceInserterJobUID)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setReferenceInserterJobUID - Invalid ctkJobDetail");
      return;
    }

    td->ReferenceInserterJobUID = referenceInserterJobUID;
  }
  QString referenceInserterJobUID(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::referenceInserterJobUID - Invalid ctkJobDetail");
      return "";
    }

    return td->ReferenceInserterJobUID;
  }

  void setQueriedPatientIDs(ctkDICOMJobDetail* td, const QStringList& queriedPatientIDs)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setQueriedPatientIDs - Invalid ctkJobDetail");
      return;
    }

    td->QueriedPatientIDs = queriedPatientIDs;
  }
  QStringList queriedPatientIDs(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::queriedPatientIDs - Invalid ctkJobDetail");
      return QStringList();
    }

    return td->QueriedPatientIDs;
  }

  void setQueriedStudyInstanceUIDs(ctkDICOMJobDetail* td, const QStringList& queriedStudyInstanceUIDs)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setQueriedStudyInstanceUIDs - Invalid ctkJobDetail");
      return;
    }

    td->QueriedStudyInstanceUIDs = queriedStudyInstanceUIDs;
  }
  QStringList queriedStudyInstanceUIDs(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::queriedStudyInstanceUIDs - Invalid ctkJobDetail");
      return QStringList();
    }

    return td->QueriedStudyInstanceUIDs;
  }

  void setQueriedSeriesInstanceUIDs(ctkDICOMJobDetail* td, const QStringList& queriedSeriesInstanceUIDs)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setQueriedSeriesInstanceUIDs - Invalid ctkJobDetail");
      return;
    }

    td->QueriedSeriesInstanceUIDs = queriedSeriesInstanceUIDs;
  }
  QStringList queriedSeriesInstanceUIDs(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::queriedSeriesInstanceUIDs - Invalid ctkJobDetail");
      return QStringList();
    }

    return td->QueriedSeriesInstanceUIDs;
  }

  void setQueriedSOPInstanceUIDs(ctkDICOMJobDetail* td, const QStringList& queriedSOPInstanceUIDs)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setQueriedSOPInstanceUIDs - Invalid ctkJobDetail");
      return;
    }

    td->QueriedSOPInstanceUIDs = queriedSOPInstanceUIDs;
  }
  QStringList queriedSOPInstanceUIDs(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::queriedSOPInstanceUIDs - Invalid ctkJobDetail");
      return QStringList();
    }

    return td->QueriedSOPInstanceUIDs;
  }

  void setConnectionName(ctkDICOMJobDetail* td, const QString& connectionName)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setConnectionName - Invalid ctkJobDetail");
      return;
    }

    td->ConnectionName = connectionName;
  }
  QString connectionName(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::connectionName - Invalid ctkJobDetail");
      return "";
    }

    return td->ConnectionName;
  }

  void setDICOMLevel(ctkDICOMJobDetail* td, ctkDICOMJob::DICOMLevels level)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setDICOMLevel - Invalid ctkJobDetail");
      return;
    }

    td->DICOMLevel = level;
  }
  ctkDICOMJob::DICOMLevels DICOMLevel(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::DICOMLevel - Invalid ctkJobDetail");
      return ctkDICOMJob::DICOMLevels::None;
    }

    return td->DICOMLevel;
  }

  void setJobType(ctkDICOMJobDetail* td, ctkDICOMJobResponseSet::JobType jobType)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setJobType - Invalid ctkJobDetail");
      return;
    }

    td->JobType = jobType;
  }
  ctkDICOMJobResponseSet::JobType jobType(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::jobType - Invalid ctkJobDetail");
      return ctkDICOMJobResponseSet::JobType::None;
    }

    return td->JobType;
  }

  void setNumberOfDataSets(ctkDICOMJobDetail* td, int numberOfDataSets)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::setNumberOfDataSets - Invalid ctkJobDetail");
      return;
    }

    td->NumberOfDataSets = numberOfDataSets;
  }
  int numberOfDataSets(ctkDICOMJobDetail* td)
  {
    if (td == nullptr)
    {
      logger.error("ctkDICOMJobDetail::numberOfDataSets - Invalid ctkJobDetail");
      return -1;
    }

    return td->NumberOfDataSets;
  }

  //----------------------------------------------------------------------------
  // ctkDICOMDisplayedFieldGeneratorRuleFactory

  //----------------------------------------------------------------------------
  // static methods

  //----------------------------------------------------------------------------
  ctkDICOMDisplayedFieldGeneratorRuleFactory* static_ctkDICOMDisplayedFieldGeneratorRuleFactory_instance()
  {
    return ctkDICOMDisplayedFieldGeneratorRuleFactory::instance();
  }

  //----------------------------------------------------------------------------
  // instance methods

  //----------------------------------------------------------------------------
  bool registerDisplayedFieldGeneratorRule(ctkDICOMDisplayedFieldGeneratorRuleFactory* factory,
                                           PythonQtPassOwnershipToCPP<ctkDICOMDisplayedFieldGeneratorAbstractRule*> plugin)
  {
    return factory->registerDisplayedFieldGeneratorRule(plugin);
  }
};

//-----------------------------------------------------------------------------
class PythonQtWrapper_CTKDICOMCore : public QObject
{
  Q_OBJECT

public slots:
  ctkErrorLogLevel::LogLevel static_ctk_dicomLogLevel()
  {
    return ctk::dicomLogLevel();
  }

  void static_ctk_setDICOMLogLevel(ctkErrorLogLevel::LogLevel level)
  {
    ctk::setDICOMLogLevel(level);
  }

  void static_ctk_setDICOMDetailedLogging(bool enable)
  {
    ctk::setDICOMDetailedLogging(enable);
  }

  QString static_ctk_dicomLogLevelAsString()
  {
    return ctk::dicomLogLevelAsString();
  }
};

//-----------------------------------------------------------------------------
void initCTKDICOMCorePythonQtDecorators()
{
  PythonQt::self()->addDecorators(new ctkDICOMCorePythonQtDecorators);
  PythonQt::self()->registerCPPClass("ctk", "", "CTKDICOMCore", PythonQtCreateObject<PythonQtWrapper_CTKDICOMCore>);
}

#endif
