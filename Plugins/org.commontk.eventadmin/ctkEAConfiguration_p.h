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


#ifndef CTKEACONFIGURATION_P_H
#define CTKEACONFIGURATION_P_H

#include <QString>

#include "dispatch/ctkEADefaultThreadPool_p.h"
#include "ctkEventAdminService_p.h"

#include <service/cm/ctkManagedService.h>

class ctkPluginContext;
class ctkEAAbstractAdapter;

/**
 * The <code>ctkEAConfiguration</code> class encapsules the
 * configuration for the event admin.
 *
 * The service knows about the following properties which are read at plugin startup:
 * <p>
 * <p>
 *      <tt>org.commontk.eventadmin.CacheSize</tt> - The size of various internal
 *          caches.
 * </p>
 * The default value is 30. Increase in case of a large number (more then 100) of
 * <tt>ctkEventHandler</tt> services. A value less then 10 triggers the default value.
 * </p>
 * <p>
 * <p>
 *      <tt>org.commontk.eventadmin.ThreadPoolSize</tt> - The size of the thread
 *          pool.
 * </p>
 * The default value is 10. Increase in case of a large amount of synchronous events
 * where the <tt>ctkEventHandler</tt> services in turn send new synchronous events in
 * the event dispatching thread or a lot of timeouts are to be expected. A value of
 * less then 2 triggers the default value. A value of 2 effectively disables thread
 * pooling.
 * </p>
 * <p>
 * <p>
 *      <tt>org.commontk.eventadmin.Timeout</tt> - The black-listing timeout in
 *          milliseconds
 * </p>
 * The default value is 5000. Increase or decrease at own discretion. A value of less
 * then 100 turns timeouts off. Any other value is the time in milliseconds granted
 * to each <tt>ctkEventHandler</tt> before it gets blacklisted.
 * </p>
 * <p>
 * <p>
 *      <tt>org.commontk.eventadmin.RequireTopic</tt> - Are <tt>ctkEventHandler</tt>
 *          required to be registered with a topic?
 * </p>
 * The default is <tt>true</tt>. The specification says that <tt>ctkEventHandler</tt>
 * must register with a list of topics they are interested in. Setting this value to
 * <tt>false</tt> will enable that handlers without a topic are receiving all events
 * (i.e., they are treated the same as with a topic=*).
 * </p>
 * <p>
 * <p>
 *      <tt>org.commontk.eventadmin.IgnoreTimeout</tt> - Configure
 *         <tt>ctkEventHandler</tt>s to be called without a timeout.
 * </p>
 * If a timeout is configured by default all event handlers are called using the timeout.
 * For performance optimization it is possible to configure event handlers where the
 * timeout handling is not used - this reduces the thread usage from the thread pools
 * as the timout handling requires an additional thread to call the event handler.
 * However, the application should work without this configuration property. It is a
 * pure optimization!
 * The value is a list of strings (separated by comma) which is assumed to define
 * exact class names.
 *
 * These properties are read at startup and serve as a default configuration.
 * If a configuration admin is configured, the event admin can be configured
 * through the config admin.
 */
class ctkEAConfiguration : public QObject, public ctkManagedService
{
  Q_OBJECT
  Q_INTERFACES(ctkManagedService)

public:

  /** The PID for the event admin. */
  static const QString PID; // = "org.commontk.eventadmin.impl.EventAdmin"

  static const QString PROP_CACHE_SIZE; // = "org.commontk.eventadmin.CacheSize"
  static const QString PROP_THREAD_POOL_SIZE; // = "org.commontk.eventadmin.ThreadPoolSize"
  static const QString PROP_TIMEOUT; // = "org.commontk.eventadmin.Timeout"
  static const QString PROP_REQUIRE_TOPIC; // = "org.commontk.eventadmin.RequireTopic"
  static const QString PROP_IGNORE_TIMEOUT; // = "org.commontk.eventadmin.IgnoreTimeout"
  static const QString PROP_LOG_LEVEL; // = "org.commontk.eventadmin.LogLevel"

private:

  QMutex mutex;

  /** The plugin context. */
  ctkPluginContext* pluginContext;

  int cacheSize;

  int threadPoolSize;

  int timeout;

  bool requireTopic;

  QStringList ignoreTimeout;

  int logLevel;

  // The thread pool used - this is a member because we need to close it on stop
  ctkEADefaultThreadPool* sync_pool;
  ctkEADefaultThreadPool* async_pool;

  // The actual implementation of the service - this is a member because we need to
  // close it on stop. Note, security is not part of this implementation but is
  // added via a decorator in the start method (this is the wrapped object without
  // the wrapper).
  ctkEventAdminService* admin;

  QScopedPointer<QObject> metaTypeService;

  // The registration of the security decorator factory (i.e., the service)
  ctkServiceRegistration registration;

  // all adapters
  QList<ctkEAAbstractAdapter*> adapters;

  ctkServiceRegistration managedServiceReg;

public:

  ctkEAConfiguration(ctkPluginContext* pluginContext);
  ~ctkEAConfiguration();

  void updated(const ctkDictionary& properties);

  void updateFromConfigAdmin(const ctkDictionary& config);

  /**
   * Configures this instance.
   */
  void configure(const ctkDictionary& config);

  /**
   * Called upon stopping the plugin. This will block until all pending events are
   * delivered. An ctkIllegalStateException will be thrown on new events starting with
   * the begin of this method. However, it might take some time until we settle
   * down which is somewhat cumbersome given that the spec asks for return in
   * a timely manner.
   */
  void destroy();

private:

  void startOrUpdate();

  /**
   * Init the adapters in org.commontk.eventadmin.impl.adapter
   */
  void adaptEvents(ctkEventAdmin* admin);

  QObject* tryToCreateMetaTypeProvider(ctkManagedService* managedService);

  /**
   * Returns either the parsed int from the value of the property if it is set and
   * not less then the min value or the default. Additionally, a warning is
   * generated in case the value is erroneous (i.e., can not be parsed as an int or
   * is less then the min value).
   */
  int getIntProperty(const QString& key, const QVariant& value,
                     int defaultValue, int min);

  /**
   * Returns true if the value of the property is set and is either 1, true, or yes
   * Returns false if the value of the property is set and is either 0, false, or no
   * Returns the defaultValue otherwise
   */
  bool getBoolProperty(const QVariant& obj, bool defaultValue);
};


#endif // CTKEACONFIGURATION_P_H
