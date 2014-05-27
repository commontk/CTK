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


#ifndef CTKMETATYPEACTIVATOR_P_H
#define CTKMETATYPEACTIVATOR_P_H

#include <ctkPluginActivator.h>

#include <service/log/ctkLogService.h>
#include <ctkServiceTracker.h>

class ctkMTLogTracker;
class ctkMetaTypeServiceImpl;

class ctkMetaTypeActivator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org_commontk_metatype")
#endif

private:

  QMutex mutex;

  QFile logFileFallback;

  static ctkMTLogTracker* logTracker;

  /*
   * The following filter guarantees only services meeting the following
   * criteria will be tracked.
   *
   * (1) A ctkManagedService or ctkManagedServiceFactory registered with a
   * SERVICE_PID property. May also be registered as a ctkMetaTypeProvider.
   * (2) A ctkMetaTypeProvider registered with a METATYPE_PID or
   * METATYPE_FACTORY_PID property.
   *
   * Note that it's still necessary to inspect a ctkManagedService or
   * ctkManagedServiceFactory to ensure it also implements ctkMetaTypeProvider.
   */
  static QString FILTER();
  static const QString SERVICE_PID; // = "org.commontk.metatype.impl.MetaType"

  // Could be ctkManagedService, ctkManagedServiceFactory, or ctkMetaTypeProvider.
  // The tracker tracks all services regardless of the plugin. Services are
  // filtered by plugin later in the ctkMTProviderTracker class. It may
  // therefore be shared among multiple instances of that class.
  ctkServiceTracker<>* metaTypeProviderTracker;

  ctkMetaTypeServiceImpl* metaTypeService;
  ctkServiceRegistration metaTypeServiceRegistration;

public:

  ctkMetaTypeActivator();
  ~ctkMetaTypeActivator();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

  /**
   * Gets a wrapper log service object. This object will either print messages
   * to standard out if no ctkLogService is present or call the respective
   * methods of available ctkLogService instances.
   */
  static ctkLogService* getLogService();

}; // ctkMetaTypeActivator

#endif // CTKMETATYPEACTIVATOR_P_H
