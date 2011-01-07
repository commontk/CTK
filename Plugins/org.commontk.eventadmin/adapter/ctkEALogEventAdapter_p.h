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


#ifndef CTKEALOGEVENTADAPTER_P_H
#define CTKEALOGEVENTADAPTER_P_H

#include <QObject>
#include <QMutex>

#include "ctkEAAbstractAdapter_p.h"

#include <ctkServiceRegistration.h>
#include <service/log/ctkLogListener.h>

class ctkEALogEventAdapter;

class _LogListener : public QObject, public ctkLogListener
{
  Q_OBJECT
  Q_INTERFACES(ctkLogListener)

public:

  _LogListener(ctkEALogEventAdapter* adapter);

  void logged(ctkLogEntryPtr entry);

private:

  ctkEALogEventAdapter* const adapter;
};

/**
 * This class registers a <tt>ctkLogListener</tt>
 * with the framework. Any received log event is then
 * posted via the ctkEventAdmin as specified in 113.6.6 OSGi R4 compendium.
 */
class ctkEALogEventAdapter : public QObject, public ctkEAAbstractAdapter
{
  Q_OBJECT

private:

  QMutex mutex;

  ctkPluginContext* context;
  _LogListener* logListener;

  ctkServiceRegistration reg;

  friend class _LogListener;

public:

  /**
   * The constructor of the adapter. This will register the listener with the
   * given context. Any received log event is then posted via the given
   * ctkEventAdmin.
   *
   * @param context The plugin context with which to register as a listener.
   * @param admin The <tt>ctkEventAdmin</tt> to use for posting events.
   */
  ctkEALogEventAdapter(ctkPluginContext* context, ctkEventAdmin* admin);

  ~ctkEALogEventAdapter();

  void destroy(ctkPluginContext* context);

};

#endif // CTKEALOGEVENTADAPTER_P_H
