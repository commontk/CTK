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


#include "ctkMTProviderTracker_p.h"

#include "ctkMTMsg_p.h"

#include <service/cm/ctkManagedService.h>
#include <service/cm/ctkManagedServiceFactory.h>
#include <service/log/ctkLogService.h>

#include <QCoreApplication>

class ctkMTProviderTracker::MetaTypeProviderWrapper
{
public:

  ctkMetaTypeProvider* provider;
  QString pid;
  bool factory;

  MetaTypeProviderWrapper(ctkMetaTypeProvider* provider, const QString& pid, bool factory)
    : provider(provider), pid(pid), factory(factory)
  {

  }

  bool operator==(const MetaTypeProviderWrapper& o) const
  {
    if (this == &o)
      return true;
    return provider == o.provider && pid == o.pid && factory == o.factory;
  }

};

uint qHash(const ctkMTProviderTracker::MetaTypeProviderWrapper wrapper)
{
  uint result = 17;
  result = 31 * result + qHash(wrapper.provider);
  result = 31 * result + qHash(wrapper.pid);
  result = 31 * result + (wrapper.factory ? 1 : 0);
  return result;
}

ctkMTProviderTracker::ctkMTProviderTracker(const QSharedPointer<ctkPlugin>& plugin, ctkLogService* log, ctkServiceTracker<>* tracker)
  : _plugin(plugin), log(log), _tracker(tracker)
{

}

QStringList ctkMTProviderTracker::getPids() const
{
  return getPids(false);
}

QStringList ctkMTProviderTracker::getFactoryPids() const
{
  return getPids(true);
}

QSharedPointer<ctkPlugin> ctkMTProviderTracker::getPlugin() const
{
  return _plugin;
}

ctkObjectClassDefinitionPtr ctkMTProviderTracker::getObjectClassDefinition(const QString& id, const QLocale& locale)
{
  if (_plugin->getState() != ctkPlugin::ACTIVE)
  {
    return ctkObjectClassDefinitionPtr(); // return none if not active
  }

  QSet<MetaTypeProviderWrapper> wrappers = getMetaTypeProviders();
  QSet<MetaTypeProviderWrapper>::ConstIterator end = wrappers.end();
  for (QSet<MetaTypeProviderWrapper>::ConstIterator it = wrappers.begin();
       it != end; ++it)
  {
    if (id == (*it).pid)
    {
      // found a matching pid now call the actual provider
      return (*it).provider->getObjectClassDefinition(id, locale);
    }
  }
  return ctkObjectClassDefinitionPtr();
}

QList<QLocale> ctkMTProviderTracker::getLocales() const
{
  if (_plugin->getState() != ctkPlugin::ACTIVE)
  {
    return QList<QLocale>(); // return none if not active
  }

  QSet<MetaTypeProviderWrapper> wrappers = getMetaTypeProviders();
  QList<QLocale> locales;
  // collect all the unique locales from all providers we found
  QSet<MetaTypeProviderWrapper>::ConstIterator end = wrappers.end();
  for (QSet<MetaTypeProviderWrapper>::ConstIterator it = wrappers.begin();
       it != end; ++it)
  {
    QList<QLocale> wrappedLocales = (*it).provider->getLocales();
    if (wrappedLocales.isEmpty())
      continue;
    for (int j = 0; j < wrappedLocales.size(); j++)
      if (!locales.contains(wrappedLocales[j]))
        locales.push_back(wrappedLocales[j]);
  }
  return locales;
}

QStringList ctkMTProviderTracker::getPids(bool factory) const
{
  if (_plugin->getState() != ctkPlugin::ACTIVE)
  {
    return QStringList(); // return none if not active
  }

  QSet<MetaTypeProviderWrapper> wrappers = getMetaTypeProviders();
  QStringList results;
  QSet<MetaTypeProviderWrapper>::ConstIterator end = wrappers.end();
  for (QSet<MetaTypeProviderWrapper>::ConstIterator it = wrappers.begin();
       it != end; ++it)
  {
    // return only the correct type of pids (regular or factory)
    if (factory == (*it).factory)
      results.push_back((*it).pid);
  }
  return results;
}

QSet<ctkMTProviderTracker::MetaTypeProviderWrapper> ctkMTProviderTracker::getMetaTypeProviders() const
{
  QMap<ctkServiceReference, QObject*> services = _tracker->getTracked();
  if (services.isEmpty())
  {
    return QSet<MetaTypeProviderWrapper>();
  }
  QList<ctkServiceReference> serviceReferences = services.keys();
  QSet<MetaTypeProviderWrapper> result;
  foreach (ctkServiceReference serviceReference, serviceReferences)
  {
    if (serviceReference.getPlugin() == _plugin)
    {
      QObject* service = services.value(serviceReference);
      // If the service is not a ctkMetaTypeProvider, we're not interested in it.
      if (ctkMetaTypeProvider* metatypeService = qobject_cast<ctkMetaTypeProvider*>(service))
      {
        // Include the METATYPE_PID, if present, to return as part of getPids(). Also, include the
        // METATYPE_FACTORY_PID, if present, to return as part of getFactoryPids().
        // The filter ensures at least one of these properties was set for a standalone ctkMetaTypeProvider.
        addMetaTypeProviderWrappers(ctkMetaTypeProvider::METATYPE_PID, serviceReference, metatypeService, false, result);
        addMetaTypeProviderWrappers(ctkMetaTypeProvider::METATYPE_FACTORY_PID, serviceReference, metatypeService, true, result);
        // If the service is a ctkManagedService, include the SERVICE_PID to return as part of getPids().
        // The filter ensures the SERVICE_PID property was set.
        if (qobject_cast<ctkManagedService*>(service))
        {
          addMetaTypeProviderWrappers(ctkPluginConstants::SERVICE_PID, serviceReference, metatypeService, false, result);
        }
        // If the service is a ctkManagedServiceFactory, include the SERVICE_PID to return as part of getFactoryPids().
        // The filter ensures the SERVICE_PID property was set.
        else if (qobject_cast<ctkManagedServiceFactory*>(service))
        {
          addMetaTypeProviderWrappers(ctkPluginConstants::SERVICE_PID, serviceReference, metatypeService, true, result);
        }
      }
    }
  }
  return result;
}

void ctkMTProviderTracker::addMetaTypeProviderWrappers(const QString& servicePropertyName,
                                                       const ctkServiceReference& serviceReference,
                                                       ctkMetaTypeProvider* service, bool factory,
                                                       QSet<MetaTypeProviderWrapper>& wrappers) const
{
  QStringList pids = getStringProperty(servicePropertyName, serviceReference.getProperty(servicePropertyName));
  foreach (QString pid, pids)
  {
    wrappers.insert(MetaTypeProviderWrapper(service, pid, factory));
  }
}

QStringList ctkMTProviderTracker::getStringProperty(const QString& name, const QVariant& value) const
{
  // Don't log a warning if the value is null. The filter guarantees at least one of the necessary properties
  // is there. If others are not, this method will get called with value equal to null.
  if (value.isNull())
    return QStringList();

  if (value.canConvert<QStringList>())
  {
    return value.toStringList();
  }

  CTK_WARN(log) << QCoreApplication::translate(ctkMTMsg::CONTEXT, ctkMTMsg::INVALID_PID_METATYPE_PROVIDER_IGNORED)
                   .arg(_plugin->getSymbolicName()).arg(_plugin->getPluginId()).arg(name). arg(value.toString());
  return QStringList();
}

