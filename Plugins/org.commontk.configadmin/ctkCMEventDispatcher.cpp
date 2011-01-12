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


#include "ctkCMEventDispatcher_p.h"

#include <service/log/ctkLogService.h>
#include <service/cm/ctkConfigurationListener.h>

#include <QRunnable>

class _DispatchEventRunnable : public QRunnable
{
public:

  _DispatchEventRunnable(ctkServiceTracker<ctkConfigurationListener*>* tracker,
                         ctkLogService* log, const ctkConfigurationEvent& event,
                         const ctkServiceReference& ref)
    : tracker(tracker), log(log), event(event), ref(ref)
  {

  }

  void run()
  {
    ctkConfigurationListener* listener = tracker->getService(ref);
    if (listener == 0)
    {
      return;
    }
    try
    {
      listener->configurationEvent(event);
    }
    catch (const std::exception& e)
    {
      log->log(ctkLogService::LOG_ERROR, e.what());
    }
    catch (...)
    {
      log->log(ctkLogService::LOG_ERROR, "Unspecified exception");
    }
  }

private:

  ctkServiceTracker<ctkConfigurationListener*>* tracker;
  ctkLogService* log;
  ctkConfigurationEvent event;
  ctkServiceReference ref;
};

ctkCMEventDispatcher::ctkCMEventDispatcher(ctkPluginContext* context, ctkLogService* log)
  : tracker(context), queue("ctkConfigurationListener Event Queue"), log(log)
{

}

void ctkCMEventDispatcher::start()
{
  tracker.open();
}

void ctkCMEventDispatcher::stop()
{
  tracker.close();
  {
    QMutexLocker lock(&mutex);
    configAdminReference = 0;
  }
}


void ctkCMEventDispatcher::setServiceReference(const ctkServiceReference& reference)
{
  QMutexLocker lock(&mutex);
  if (!configAdminReference)
  {
    configAdminReference = reference;
  }
}

void ctkCMEventDispatcher::dispatchEvent(ctkConfigurationEvent::Type type, const QString& factoryPid, const QString& pid)
{
  const ctkConfigurationEvent event = createConfigurationEvent(type, factoryPid, pid);
  if (event.isNull())
    return;

  QList<ctkServiceReference> refs = tracker.getServiceReferences();

  foreach (ctkServiceReference ref, refs)
  {
    queue.put(new _DispatchEventRunnable(&tracker, log, event, ref));
  }
}

ctkConfigurationEvent ctkCMEventDispatcher::createConfigurationEvent(ctkConfigurationEvent::Type type, const QString& factoryPid, const QString& pid)
{
  if (!configAdminReference)
  {
    return ctkConfigurationEvent();
  }

  return ctkConfigurationEvent(configAdminReference, type, factoryPid, pid);
}

