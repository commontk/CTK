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
#include <ctkDICOMUtil.h>

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
    }

public Q_SLOTS:

  //
  // None yet - refer to other libs for examples
  //

};

//-----------------------------------------------------------------------------
class PythonQtWrapper_CTKDICOMCore : public QObject
{
  Q_OBJECT

public Q_SLOTS:
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

  QString static_ctk_dicomDabataseSettingsKey()
    {
    return ctk::dicomDabataseSettingsKey();
    }

  void static_ctk_setDICOMDabataseSettingsKey(QString key)
    {
    ctk::setDICOMDabataseSettingsKey(key);
    }
};

//-----------------------------------------------------------------------------
void initCTKDICOMCorePythonQtDecorators()
{
  PythonQt::self()->addDecorators(new ctkDICOMCorePythonQtDecorators);
  PythonQt::self()->registerCPPClass("ctk", "", "CTKDICOMCore", PythonQtCreateObject<PythonQtWrapper_CTKDICOMCore>);
}

#endif
