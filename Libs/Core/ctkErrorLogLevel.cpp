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

#include "ctkErrorLogLevel.h"

// Qt includes
#include <QMetaEnum>

// --------------------------------------------------------------------------
ctkErrorLogLevel::ctkErrorLogLevel()
{
  qRegisterMetaType<ctkErrorLogLevel::LogLevel>("ctkErrorLogLevel::LogLevel");
}

// --------------------------------------------------------------------------
QString ctkErrorLogLevel::operator()(ctkErrorLogLevel::LogLevel logLevel)
{
  return ctkErrorLogLevel::logLevelAsString(logLevel);
}

// --------------------------------------------------------------------------
QString ctkErrorLogLevel::logLevelAsString(ctkErrorLogLevel::LogLevel logLevel)
{
  QMetaEnum logLevelEnum = ctkErrorLogLevel::staticMetaObject.enumerator(0);
  Q_ASSERT(QString("LogLevel").compare(logLevelEnum.name()) == 0);
  return QLatin1String(logLevelEnum.valueToKey(logLevel));
}
