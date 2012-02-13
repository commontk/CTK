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


#include "ctkLogQDebug_p.h"

#include <QDateTime>
#include <QDebug>
#include <QStringList>

#include <ctkPluginConstants.h>

ctkLogQDebug::ctkLogQDebug()
  : logLevel(ctkLogService::LOG_DEBUG)
{
}

void ctkLogQDebug::log(int level, const QString& message, const std::exception* exception,
                       const char* file, const char* function, int line)
{
  Q_UNUSED(function)

  QString s = QDateTime::currentDateTime().toString(Qt::ISODate)
      .append(" - ").append(message);

  if (exception != 0)
  {
    s.append(" (").append(exception->what()).append(")");
  }

  if (file)
  {
    s.append(" [at ").append(file).append(":").append(QString::number(line)).append("]");
  }

  if (level == ctkLogService::LOG_WARNING)
  {
    qWarning() << s;
  }
  else if (level == ctkLogService::LOG_ERROR)
  {
    qCritical() << s;
  }
  else
  {
    qDebug() << s;
  }
}

void ctkLogQDebug::log(const ctkServiceReference& sr, int level, const QString& message,
                       const std::exception* exception,
                       const char* file, const char* function, int line)
{
  Q_UNUSED(function)

  QString s = QDateTime::currentDateTime().toString(Qt::ISODate)
      .append(" - [");

  s.append(sr.getProperty(ctkPluginConstants::SERVICE_ID).toString());
  s.append(";");
  QStringList clazzes = sr.getProperty(ctkPluginConstants::OBJECTCLASS).toStringList();
  int i = 0;
  foreach (QString clazz, clazzes)
  {
    if (i > 0) s.append(",");
    s.append(clazz);
  }

  s.append(message);

  if(exception != 0)
  {
    s.append(" (").append(exception->what()).append(")");
  }

  if (file)
  {
    s.append(" [at ").append(file).append(":").append(QString::number(line)).append("]");
  }

  if (level == ctkLogService::LOG_WARNING)
  {
    qWarning() << s;
  }
  else if (level == ctkLogService::LOG_ERROR)
  {
    qCritical() << s;
  }
  else
  {
    qDebug() << s;
  }
}

int ctkLogQDebug::getLogLevel() const
{
  return logLevel;
}
