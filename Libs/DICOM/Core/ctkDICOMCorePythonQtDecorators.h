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

  void setJobType(ctkDICOMJobDetail* td, ctkDICOMJobResponseSet::JobType jobType)
  {
    td->JobType = jobType;
  }
  ctkDICOMJobResponseSet::JobType jobType(ctkDICOMJobDetail* td)
  {
    return td->JobType;
  }

  void setPatientID(ctkDICOMJobDetail* td, const QString& patientID)
  {
    td->PatientID = patientID;
  }
  QString patientID(ctkDICOMJobDetail* td)
  {
    return td->PatientID;
  }

  void setStudyInstanceUID(ctkDICOMJobDetail* td, const QString& studyInstanceUID)
  {
    td->StudyInstanceUID = studyInstanceUID;
  }
  QString studyInstanceUID(ctkDICOMJobDetail* td)
  {
    return td->StudyInstanceUID;
  }

  void setSeriesInstanceUID(ctkDICOMJobDetail* td, const QString& seriesInstanceUID)
  {
    td->SeriesInstanceUID = seriesInstanceUID;
  }
  QString seriesInstanceUID(ctkDICOMJobDetail* td)
  {
    return td->SeriesInstanceUID;
  }

  void setSOPInstanceUID(ctkDICOMJobDetail* td, const QString& sopInstanceUID)
  {
    td->SOPInstanceUID = sopInstanceUID;
  }
  QString sopInstanceUID(ctkDICOMJobDetail* td)
  {
    return td->SOPInstanceUID;
  }

  void setConnectionName(ctkDICOMJobDetail* td, const QString& connectionName)
  {
    td->ConnectionName = connectionName;
  }
  QString connectionName(ctkDICOMJobDetail* td)
  {
    return td->ConnectionName;
  }

  void setNumberOfDataSets(ctkDICOMJobDetail* td, int numberOfDataSets)
  {
    td->NumberOfDataSets = numberOfDataSets;
  }
  int numberOfDataSets(ctkDICOMJobDetail* td)
  {
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
