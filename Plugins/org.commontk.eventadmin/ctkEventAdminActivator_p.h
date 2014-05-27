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


#ifndef CTKEVENTADMINACTIVATOR_H
#define CTKEVENTADMINACTIVATOR_H

#include <ctkPluginActivator.h>

#include <service/log/ctkLogService.h>

class ctkEALogTracker;
class ctkEAConfiguration;

class ctkEventAdminActivator : public QObject,
    public ctkPluginActivator
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "org_commontk_eventadmin")
#endif

public:

  ctkEventAdminActivator();
  ~ctkEventAdminActivator();

  /**
   * Called upon starting of the plugin. Constructs and registers the ctkEventAdmin
   * service with the framework. Note that the properties of the service are
   * requested from the context in this method hence, the plugin has to be
   * restarted in order to take changed properties into account.
   *
   * @param context The plugin context passed by the framework
   *
   * @see ctkPluginActivator#start(ctkPluginContext*)
   */
  void start(ctkPluginContext* context);

  /**
   * Called upon stopping the plugin. This will block until all pending events are
   * delivered. An ctkIllegalStateException will be thrown on new events starting with
   * the begin of this method. However, it might take some time until we settle
   * down which is somewhat cumbersome given that the spec asks for return in
   * a timely manner.
   *
   * @param context The plugin context passed by the framework
   *
   * @see ctkPluginActivator#stop(ctkPluginContext*)
   */
  void stop(ctkPluginContext* context);

  /**
   * Gets a wrapper log service object. This object will either print messages
   * to standard out if no ctkLogService is present or call the respective
   * methods of available ctkLogService instances.
   */
  static ctkLogService* getLogService();

private:

  QFile logFileFallback;
  static ctkEALogTracker* logTracker;

  ctkEAConfiguration* config;
};


#endif // CTKEVENTADMINACTIVATOR_H
