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

#ifndef CTKERRORLOGLEVEL_H
#define CTKERRORLOGLEVEL_H

// Qt includes
#include <QObject>

// CTK includes
#include "ctkCoreExport.h"

//------------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkErrorLogLevel : public QObject
{
  Q_OBJECT
  Q_FLAGS(LogLevel)
public:
  ctkErrorLogLevel();

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

  QString operator ()(LogLevel logLevel);

  static QString logLevelAsString(ctkErrorLogLevel::LogLevel logLevel);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkErrorLogLevel::LogLevels)


#endif // CTKERRORLOGLEVEL_H
