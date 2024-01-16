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

// CTK includes
#include <ctkDICOMDisplayedFieldGeneratorRuleFactory.h>
#include <ctkDICOMUtil.h>
#include <ctkDICOMJobResponseSet.h>

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

/// \ingroup DICOM_Core
class ctkDICOMCorePythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkDICOMCorePythonQtDecorators()
    {
    PythonQt::self()->registerCPPClass("ctkJobDetail", 0, "CTKDICOMCore");
    }

public slots:

  //----------------------------------------------------------------------------
  // ctkJobDetail
  //----------------------------------------------------------------------------
  ctkJobDetail* new_ctkJobDetail()
    {
    return new ctkJobDetail();
    }

  void setTypeOfJob(ctkJobDetail* td, ctkDICOMJobResponseSet::JobType jobType)
    {
    td->TypeOfJob = jobType;
    }
  ctkDICOMJobResponseSet::JobType TypeOfJob(ctkJobDetail* td)
    {
    return td->TypeOfJob;
    }

  void setJobUID(ctkJobDetail* td, const QString& jobUID)
    {
    td->JobUID = jobUID;
    }
  QString JobUID(ctkJobDetail* td)
    {
    return td->JobUID;
    }

  void setPatientID(ctkJobDetail* td, const QString& patientID)
    {
    td->PatientID = patientID;
    }
  QString patientID(ctkJobDetail* td)
    {
    return td->PatientID;
    }

  void setStudyInstanceUID(ctkJobDetail* td, const QString& studyInstanceUID)
    {
    td->StudyInstanceUID = studyInstanceUID;
    }
  QString studyInstanceUID(ctkJobDetail* td)
    {
    return td->StudyInstanceUID;
    }

  void setSeriesInstanceUID(ctkJobDetail* td, const QString& seriesInstanceUID)
    {
    td->SeriesInstanceUID = seriesInstanceUID;
    }
  QString seriesInstanceUID(ctkJobDetail* td)
    {
    return td->SeriesInstanceUID;
    }

  void setSOPInstanceUID(ctkJobDetail* td, const QString& sopInstanceUID)
    {
    td->SOPInstanceUID = sopInstanceUID;
    }
  QString sopInstanceUID(ctkJobDetail* td)
    {
    return td->SOPInstanceUID;
    }

  void setConnectionName(ctkJobDetail* td, const QString& connectionName)
    {
    td->ConnectionName = connectionName;
    }
  QString connectionName(ctkJobDetail* td)
    {
    return td->ConnectionName;
    }

  void setNumberOfDataSets(ctkJobDetail* td, int numberOfDataSets)
    {
    td->NumberOfDataSets = numberOfDataSets;
    }
  int numberOfDataSets(ctkJobDetail* td)
    {
    return td->NumberOfDataSets;
    }

  //----------------------------------------------------------------------------
  // ctkDICOMJobResponseSet
  //----------------------------------------------------------------------------
  void setFilePath(ctkDICOMJobResponseSet* ts, const QString& filePath)
    {
    ts->setFilePath(filePath);
    }

  void setCopyFile(ctkDICOMJobResponseSet* ts, bool copyFile)
    {
    ts->setCopyFile(copyFile);
    }

  void setOverwriteExistingDataset(ctkDICOMJobResponseSet* ts, bool overwriteExistingDataset)
    {
    ts->setOverwriteExistingDataset(overwriteExistingDataset);
    }

  void setTypeOfJob(ctkDICOMJobResponseSet* ts, ctkDICOMJobResponseSet::JobType TypeOfJob)
    {
    ts->setTypeOfJob(TypeOfJob);
    }

  void setJobUID(ctkDICOMJobResponseSet* ts, const QString& jobUID)
    {
    ts->setJobUID(jobUID);
    }

  void setPatientID(ctkDICOMJobResponseSet* ts, const QString& patientID)
    {
    ts->setPatientID(patientID);
    }

  void setStudyInstanceUID(ctkDICOMJobResponseSet* ts, const QString& studyInstanceUID)
    {
    ts->setStudyInstanceUID(studyInstanceUID);
    }

  void setSeriesInstanceUID(ctkDICOMJobResponseSet* ts, const QString& seriesInstanceUID)
    {
    ts->setSeriesInstanceUID(seriesInstanceUID);
    }

  void setSOPInstanceUID(ctkDICOMJobResponseSet* ts, const QString& sopInstanceUID)
    {
    ts->setSOPInstanceUID(sopInstanceUID);
    }

  void setConnectionName(ctkDICOMJobResponseSet* ts, const QString& connectionName)
    {
    ts->setConnectionName(connectionName);
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
