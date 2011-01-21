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


#ifndef CTKCMEVENTDISPATCHER_P_H
#define CTKCMEVENTDISPATCHER_P_H

#include <ctkServiceTracker.h>
#include <service/cm/ctkConfigurationEvent.h>

#include "ctkCMSerializedTaskQueue_p.h"

struct ctkConfigurationListener;
struct ctkLogService;

/**
 * EventDispatcher is responsible for delivering Configuration Events to ConfigurationListeners.
 * The originating ConfigAdmin ServiceReference is needed when delivering events. This reference
 * is made available by the service factory before returning the service object.
 */
class ctkCMEventDispatcher
{

public:

  ctkCMEventDispatcher(ctkPluginContext* context, ctkLogService* log);

  void start();
  void stop();

  void setServiceReference(const ctkServiceReference& reference);

  void dispatchEvent(ctkConfigurationEvent::Type type, const QString& factoryPid, const QString& pid);

private:

  QMutex mutex;
  ctkServiceTracker<ctkConfigurationListener*> tracker;
  ctkCMSerializedTaskQueue queue;
  /** @GuardedBy mutex */
  ctkServiceReference configAdminReference;
  ctkLogService * const log;

  ctkConfigurationEvent createConfigurationEvent(ctkConfigurationEvent::Type type, const QString& factoryPid, const QString& pid);
};

#endif // CTKCMEVENTDISPATCHER_P_H
