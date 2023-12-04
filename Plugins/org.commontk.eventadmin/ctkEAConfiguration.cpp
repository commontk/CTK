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


#include "ctkEAConfiguration_p.h"

#include "ctkEventAdminService_p.h"
#include "ctkEAMetaTypeProvider_p.h"
#include "adapter/ctkEAFrameworkEventAdapter_p.h"
#include "adapter/ctkEALogEventAdapter_p.h"
#include "adapter/ctkEAPluginEventAdapter_p.h"
#include "adapter/ctkEAServiceEventAdapter_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <QtConcurrentRun>

const QString ctkEAConfiguration::PID = "org.commontk.eventadmin.impl.EventAdmin";

const QString ctkEAConfiguration::PROP_CACHE_SIZE = "org.commontk.eventadmin.CacheSize";
const QString ctkEAConfiguration::PROP_THREAD_POOL_SIZE = "org.commontk.eventadmin.ThreadPoolSize";
const QString ctkEAConfiguration::PROP_TIMEOUT = "org.commontk.eventadmin.Timeout";
const QString ctkEAConfiguration::PROP_REQUIRE_TOPIC = "org.commontk.eventadmin.RequireTopic";
const QString ctkEAConfiguration::PROP_IGNORE_TIMEOUT = "org.commontk.eventadmin.IgnoreTimeout";
const QString ctkEAConfiguration::PROP_LOG_LEVEL = "org.commontk.eventadmin.LogLevel";


ctkEAConfiguration::ctkEAConfiguration(ctkPluginContext* pluginContext )
  : pluginContext(pluginContext), sync_pool(0), async_pool(0), admin(0)
{
  // default configuration
  configure(ctkDictionary());
  startOrUpdate();

  try
  {
    QObject* service = this;

    // add meta type provider
    metaTypeService.reset(tryToCreateMetaTypeProvider(qobject_cast<ctkManagedService*>(service)));
    QStringList interfaceNames;
    if (metaTypeService.isNull())
    {
      interfaceNames.append(getIIDs<ctkManagedService>());
    }
    else
    {
      interfaceNames.append(getIIDs<ctkManagedService, ctkMetaTypeProvider>());
      service = metaTypeService.data();
    }
    ctkDictionary props;
    props.insert(ctkPluginConstants::SERVICE_PID, PID);
    managedServiceReg = pluginContext->registerService(interfaceNames, service, props);
  }
  catch (...)
  {
    // don't care
  }
}

ctkEAConfiguration::~ctkEAConfiguration()
{
  this->destroy();
}

void ctkEAConfiguration::updateFromConfigAdmin(const ctkDictionary& config)
{
  QMutexLocker l(&mutex);
  configure(config);
  startOrUpdate();
}

void ctkEAConfiguration::configure(const ctkDictionary& config )
{
  if (config.isEmpty())
  {
    // The size of various internal caches. At the moment there are 4
    // internal caches affected. Each will cache the determined amount of
    // small but frequently used objects (i.e., in case of the default value
    // we end-up with a total of 120 small objects being cached). A value of less
    // then 10 triggers the default value.
    cacheSize = getIntProperty(PROP_CACHE_SIZE,
                               pluginContext->getProperty(PROP_CACHE_SIZE), 30, 10);

    // The size of the internal thread pool. Note that we must execute
    // each synchronous event dispatch that happens in the synchronous event
    // dispatching thread in a new thread, hence a small thread pool is o.k.
    // A value of less then 2 triggers the default value. A value of 2
    // effectively disables thread pooling. Furthermore, this will be used by
    // a lazy thread pool (i.e., new threads are created when needed). Ones the
    // the size is reached and no cached thread is available new threads will
    // be created.
    threadPoolSize = getIntProperty(PROP_THREAD_POOL_SIZE,
                                    pluginContext->getProperty(PROP_THREAD_POOL_SIZE), 20, 2);

    // The timeout in milliseconds - A value of less then 100 turns timeouts off.
    // Any other value is the time in milliseconds granted to each EventHandler
    // before it gets blacklisted.
    timeout = getIntProperty(PROP_TIMEOUT,
                             pluginContext->getProperty(PROP_TIMEOUT), 5000, INT_MIN);

    // Are EventHandler required to be registered with a topic? - The default is
    // true. The specification says that EventHandler must register with a list
    // of topics they are interested in. Setting this value to false will enable
    // that handlers without a topic are receiving all events
    // (i.e., they are treated the same as with a topic=*).
    requireTopic = getBoolProperty(pluginContext->getProperty(PROP_REQUIRE_TOPIC), true);
    QVariant value = pluginContext->getProperty(PROP_IGNORE_TIMEOUT);
    if (value.isValid())
    {
      ignoreTimeout = value.toStringList();
    }
    else
    {
      ignoreTimeout.clear();
    }
    logLevel = getIntProperty(PROP_LOG_LEVEL,
                              pluginContext->getProperty(PROP_LOG_LEVEL),
                              ctkLogService::LOG_WARNING, // default log level is WARNING
                              ctkLogService::LOG_ERROR);
  }
  else
  {
    cacheSize = getIntProperty(PROP_CACHE_SIZE, config.value(PROP_CACHE_SIZE), 30, 10);
    threadPoolSize = getIntProperty(PROP_THREAD_POOL_SIZE, config.value(PROP_THREAD_POOL_SIZE), 20, 2);
    timeout = getIntProperty(PROP_TIMEOUT, config.value(PROP_TIMEOUT), 5000, INT_MIN);
    requireTopic = getBoolProperty(config.value(PROP_REQUIRE_TOPIC), true);
    ignoreTimeout.clear();
    QVariant value = config.value(PROP_IGNORE_TIMEOUT);
    if (value.canConvert<QStringList>())
    {
      ignoreTimeout = value.toStringList();
    }
    else
    {
      CTK_WARN(ctkEventAdminActivator::getLogService())
          << "Value for property:" << PROP_IGNORE_TIMEOUT << " cannot be converted to QStringList - Using default";
    }
    logLevel = getIntProperty(PROP_LOG_LEVEL,
                              config.value(PROP_LOG_LEVEL),
                              ctkLogService::LOG_WARNING, // default log level is WARNING
                              ctkLogService::LOG_ERROR);
  }
  // a timeout less or equals to 100 means : disable timeout
  if (timeout <= 100)
  {
    timeout = 0;
  }
}

void ctkEAConfiguration::destroy()
{
  QMutexLocker l(&mutex);
  if (!adapters.isEmpty())
  {
    foreach(ctkEAAbstractAdapter* adapter, adapters)
    {
      adapter->destroy(pluginContext);
      delete adapter;
    }
    adapters.clear();
  }
  if (managedServiceReg)
  {
    managedServiceReg.unregister();
    managedServiceReg = 0;
  }
  // We need to unregister manually
  if (registration)
  {
    registration.unregister();
    registration = 0;
  }
  if (admin)
  {
    admin->stop();
    delete admin;
    admin = 0;
  }
  if (async_pool)
  {
    async_pool->close();
    delete async_pool;
    async_pool = 0;
  }
  if (sync_pool)
  {
    sync_pool->close();
    delete sync_pool;
    sync_pool = 0;
  }
}

void ctkEAConfiguration::startOrUpdate()
{
  CTK_DEBUG(ctkEventAdminActivator::getLogService())
      << PROP_LOG_LEVEL << "=" << logLevel;
  CTK_DEBUG(ctkEventAdminActivator::getLogService())
      << PROP_CACHE_SIZE << "=" << cacheSize;
  CTK_DEBUG(ctkEventAdminActivator::getLogService())
      << PROP_THREAD_POOL_SIZE << "=" << threadPoolSize;
  CTK_DEBUG(ctkEventAdminActivator::getLogService())
      << PROP_TIMEOUT << "=" << timeout;
  CTK_DEBUG(ctkEventAdminActivator::getLogService())
      << PROP_REQUIRE_TOPIC << "=" << requireTopic;

  ctkEventAdminService::TopicHandlerFiltersInterface* topicHandlerFilters =
      new ctkEventAdminService::TopicHandlerFilters(
        new ctkEventAdminService::TopicCacheMap(cacheSize), requireTopic);

  ctkEventAdminService::FiltersInterface* filters =
      new ctkEventAdminService::Filters(
        new ctkEventAdminService::LDAPCacheMap(cacheSize), pluginContext);

  // Note that this uses a lazy thread pool that will create new threads on
  // demand - in case none of its cached threads is free - until threadPoolSize
  // is reached. Subsequently, a threadPoolSize of 2 effectively disables
  // caching of threads.
  if (sync_pool == 0)
  {
    sync_pool = new ctkEADefaultThreadPool(threadPoolSize, true);
  }
  else
  {
    sync_pool->configure(threadPoolSize);
  }

  int asyncThreadPoolSize = threadPoolSize > 5 ? threadPoolSize / 2 : 2;
  if (async_pool == 0)
  {
    async_pool = new ctkEADefaultThreadPool(asyncThreadPoolSize, false);
  }
  else
  {
    async_pool->configure(asyncThreadPoolSize);
  }

  // The handlerTasks object is responsible to determine concerned ctkEventHandler
  // for a given event. Additionally, it keeps a list of blacklisted handlers.
  // Note that blacklisting is deactivated by selecting a different scheduler
  // below (and not in this HandlerTasks object!)
  ctkEventAdminService::HandlerTasksInterface* handlerTasks =
      new ctkEventAdminService::BlacklistingHandlerTasks(
        pluginContext, new ctkEventAdminService::BlackList(), topicHandlerFilters, filters);

  if (admin == 0)
  {
    admin = new ctkEventAdminService(pluginContext, handlerTasks, sync_pool, async_pool,
                                     timeout, ignoreTimeout);

    // Finally, adapt the outside events to our kind of events as per spec
    adaptEvents(admin);

    // register the admin wrapped in a service factory (SecureEventAdminFactory)
    // that hands-out the m_admin object wrapped in a decorator that checks
    // appropriated permissions of each calling plugin
    //TODO SecureEventAdminFactory
    //registration = pluginContext->registerService<ctkEventAdmin>(
    //      new ctkEASecureEventAdminFactory(admin));
    registration = pluginContext->registerService<ctkEventAdmin>(admin);
  }
  else
  {
    admin->update(handlerTasks, timeout, ignoreTimeout);
  }

}

void ctkEAConfiguration::adaptEvents(ctkEventAdmin* admin)
{
  adapters << new ctkEAFrameworkEventAdapter(pluginContext, admin);
  adapters << new ctkEAPluginEventAdapter(pluginContext, admin);
  adapters << new ctkEAServiceEventAdapter(pluginContext, admin);
  adapters << new ctkEALogEventAdapter(pluginContext, admin);
}

QObject* ctkEAConfiguration::tryToCreateMetaTypeProvider(ctkManagedService* managedService)
{
  try
  {
    return new ctkEAMetaTypeProvider(managedService, cacheSize, threadPoolSize,
                                     timeout, requireTopic, ignoreTimeout);
  }
  catch (...)
  {
    // we simply ignore this
  }
  return 0;
}

void ctkEAConfiguration::updated(const ctkDictionary& properties)
{
  // do this in the background as we don't want to stop
  // the config admin
  QFuture<void> future = QtConcurrent::run([=]() { updateFromConfigAdmin(properties); });
}

int ctkEAConfiguration::getIntProperty(const QString& key, const QVariant& value,
                                       int defaultValue, int min)
{
  if(value.isValid())
  {
    int result = defaultValue;
    if (value.canConvert<int>())
    {
      bool ok;
      result = value.toInt(&ok);
      if (!ok)
      {
        CTK_WARN(ctkEventAdminActivator::getLogService())
            << "Unable to parse property: " << key << " - Using default: " << defaultValue;
        return defaultValue;
      }
    }
    else
    {
      CTK_WARN(ctkEventAdminActivator::getLogService())
          << "Unable to convert property to int: " << key << " - Using default: " << defaultValue;
      return defaultValue;
    }

    if(result >= min)
    {
      return result;
    }

    CTK_WARN(ctkEventAdminActivator::getLogService())
        << "Value for property: " << key << " is too low - Using default: " << defaultValue;
  }

  return defaultValue;
}

bool ctkEAConfiguration::getBoolProperty(const QVariant& value, bool defaultValue)
{
  if(value.isValid())
  {
    if (value.canConvert<bool>())
    {
      return value.toBool();
    }
  }

  return defaultValue;
}
