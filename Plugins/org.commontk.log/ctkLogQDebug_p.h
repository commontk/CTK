/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#ifndef CTKLOGQDEBUG_P_H
#define CTKLOGQDEBUG_P_H

#include <service/log/ctkLogService.h>

#include <QObject>

class ctkLogQDebug : public QObject, public ctkLogService
{

  Q_OBJECT
  Q_INTERFACES(ctkLogService)

public:

  ctkLogQDebug();

  void log(int level, const QString& message, const std::exception* exception = 0,
           const char* file = 0, const char* function = 0, int line = -1);
  void log(const ctkServiceReference& sr, int level, const QString& message,
           const std::exception* exception = 0,
           const char* file = 0, const char* function = 0, int line = -1);
  int getLogLevel() const;

private:

  int logLevel;
};

#endif // CTKLOGQDEBUG_P_H
