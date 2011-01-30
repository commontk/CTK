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


#include "ctkCMLogTracker_p.h"

#include <QDateTime>

ctkCMLogTracker::ctkCMLogTracker(ctkPluginContext* context, QIODevice* out)
  : ctkServiceTracker<ctkLogService*>(context), out(out), logLevel(std::numeric_limits<int>::max())
{

}

int ctkCMLogTracker::getLogLevel() const
{
  return logLevel;
}

ctkLogService* ctkCMLogTracker::addingService(const ctkServiceReference& reference)
{
  ctkLogService* service = getService(reference);
  if (logLevel == std::numeric_limits<int>::max() ||
      service->getLogLevel() > logLevel)
  {
    logLevel = service->getLogLevel();
  }
  return service;
}

void ctkCMLogTracker::removedService(const ctkServiceReference& reference, ctkLogService* service)
{
  logLevel = std::numeric_limits<int>::max();

  QList<ctkLogService*> services = getServices();
  foreach (ctkLogService* s, services)
  {
    if (s == service) continue;
    if (logLevel == std::numeric_limits<int>::max() ||
        s->getLogLevel() > logLevel)
    {
      logLevel = s->getLogLevel();
    }
  }

  ctkServiceTracker<ctkLogService*>::removedService(reference, service);
}

void ctkCMLogTracker::log(int level, const QString& message, const std::exception* exception,
                          const char* file, const char* function, int line)
{
  logToAll(ctkServiceReference(), level, message, exception, file, function, line);
}

void ctkCMLogTracker::log(const ctkServiceReference& reference, int level, const QString& message,
                          const std::exception* exception, const char* file, const char* function, int line)
{
  logToAll(reference, level, message, exception, file, function, line);
}

void ctkCMLogTracker::logToAll(const ctkServiceReference& reference, int level, const QString& message,
                               const std::exception* exception, const char* file, const char* function, int line)
{
  QMutexLocker lock(&mutex);
  QList<ctkServiceReference> references = getServiceReferences();

  if (!references.isEmpty())
  {
    foreach (ctkServiceReference ref, references)
    {
      ctkLogService* service = getService(ref);
      if (service != 0)
      {
        try
        {
          service->log(reference, level, message, exception, file, function, line);
        }
        catch (...)
        {
          // TODO: consider printing to System Error
        }
      }
    }
    return;
  }

  noLogService(reference, level, message, exception, file, function, line);
}

void ctkCMLogTracker::noLogService(const ctkServiceReference& reference, int level, const QString& message,
                                   const std::exception* exc, const char* file, const char* function, int line)
{
  Q_UNUSED(reference)

  QString timestamp = QDateTime::currentDateTime().toString();

  out << timestamp;

  if (level == ctkLogService::LOG_DEBUG)
  {
    out << " Debug";
  }
  else if (level == LOG_INFO)
  {
    out << " Info";
  }
  else if (level == LOG_WARNING)
  {
    out << " Warning";
  }
  else if (level == LOG_ERROR)
  {
    out << " Error";
  }
  else
  {
    out << "[Unknown Log Level]";
  }

  out << ": " << message << '\n';

  if (reference)
  {
    out << reference << '\n';
  }

  if (exc)
  {
    out << exc->what() << '\n';
  }

  if (file || function)
  {
    out << "in " << function << ", " << file << ":" << line << '\n';
  }

  out.flush();
}
