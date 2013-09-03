/*=========================================================================

  Library:   CTK

  Copyright (c) Brigham & Women's Hospital

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

#ifndef __ctkDICOMUtil_h
#define __ctkDICOMUtil_h

// Qt includes 
#include <QObject>
#include <QString>

// CTK includes
#include "ctkDICOMCoreExport.h"
#include "ctkDICOMDatabase.h"

class ctkDICOMUtilPrivate;

class CTK_DICOM_CORE_EXPORT ctkDICOMUtil : public QObject
{
  Q_OBJECT
  Q_FLAGS(LogLevel)
public:
   enum LogLevel
    {
    None     = 0x0,
    Unknown  = 0x1,
    Status   = 0x2,
    Trace    = 0x4,
    Debug    = 0x8,
    Info     = 0x10,
    Warning  = 0x20,
    Error    = 0x40,
    Critical = 0x80,
    Fatal    = 0x100
    };
  Q_DECLARE_FLAGS(LogLevels, LogLevel)
  Q_ENUMS(LogLevel)
 
  explicit ctkDICOMUtil(QObject* parent = 0);
  virtual ~ctkDICOMUtil();

  Q_INVOKABLE void setDICOMLogLevel(LogLevel level);
  Q_INVOKABLE LogLevel getDICOMLogLevel() const;
  Q_INVOKABLE QString DICOMLogLevel() const;

protected:
  QScopedPointer<ctkDICOMUtilPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMUtil);
  Q_DISABLE_COPY(ctkDICOMUtil);
};
#endif
