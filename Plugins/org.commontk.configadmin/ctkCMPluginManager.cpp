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


#include "ctkCMPluginManager_p.h"

#include <ctkServiceTracker.h>
#include <service/cm/ctkConfigurationPlugin.h>

#include <QStringList>

#include <set>

struct _PluginTrackerCompare
{
  bool operator() (const ctkServiceReference& l, const ctkServiceReference& r) const
  {
    return getRank(l) < getRank(r);
  }

  int getRank(const ctkServiceReference& ref) const
  {
    QVariant ranking = ref.getProperty(ctkConfigurationPlugin::CM_RANKING);
    if (!ranking.isValid() || !(ranking.canConvert<int>()))
      return 0;
    return ranking.toInt();
  }
};

class _PluginTracker : public ctkServiceTracker<ctkConfigurationPlugin*>
{
  mutable QMutex mutex;
  typedef std::set<ctkServiceReference, _PluginTrackerCompare> ServiceRefContainer;
  ServiceRefContainer serviceReferences;
  ctkPluginContext* context;

public:

  _PluginTracker(ctkPluginContext* context)
    : ctkServiceTracker<ctkConfigurationPlugin*>(context),
      context(context)
  {
  }

  QList<ctkServiceReference> getServiceReferences() const
  {
    QMutexLocker lock(&mutex);
    QList<ctkServiceReference> refs;
    ServiceRefContainer::const_iterator refEnd = serviceReferences.end();
    for (ServiceRefContainer::const_iterator it = serviceReferences.begin();
         it != refEnd; ++it)
    {
      refs.push_back(*it);
    }
    return refs;
  }

  ctkConfigurationPlugin* addingService(const ctkServiceReference& reference)
  {
    QMutexLocker lock(&mutex);
    serviceReferences.insert(reference);
    return context->getService<ctkConfigurationPlugin>(reference);
  }

  void modifiedService(const ctkServiceReference& reference, ctkConfigurationPlugin* service)
  {
    Q_UNUSED(reference)
    Q_UNUSED(service)
    // nothing to do
  }

  void removedService(const ctkServiceReference& reference, ctkConfigurationPlugin* service)
  {
    Q_UNUSED(service)

    QMutexLocker lock(&mutex);
    serviceReferences.erase(reference);
    context->ungetService(reference);
  }
};

ctkCMPluginManager::ctkCMPluginManager(ctkPluginContext* context)
{
  pluginTracker.reset(new _PluginTracker(context));
}

ctkCMPluginManager::~ctkCMPluginManager()
{

}

void ctkCMPluginManager::start()
{
  pluginTracker->open();
}

void ctkCMPluginManager::stop()
{
  pluginTracker->close();
}

void ctkCMPluginManager::modifyConfiguration(const ctkServiceReference& managedReference, ctkDictionary& properties)
{
  if (properties.empty())
    return;

  QList<ctkServiceReference> references = pluginTracker->getServiceReferences();
  foreach(ctkServiceReference ref, references)
  {
    QVariant pids = ref.getProperty(ctkConfigurationPlugin::CM_TARGET);
    if (pids.isValid() && pids.canConvert<QStringList>())
    {
      QString pid = properties.value(ctkPluginConstants::SERVICE_PID).toString();
      if (!pids.toStringList().contains(pid))
        continue;
    }
    ctkConfigurationPlugin* plugin = pluginTracker->getService(ref);
    if (plugin != 0)
      plugin->modifyConfiguration(managedReference, properties);
  }
}
