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


#ifndef CTKEAFRAMEWORKEVENTADAPTER_P_H
#define CTKEAFRAMEWORKEVENTADAPTER_P_H

#include <QObject>

#include "ctkEAAbstractAdapter_p.h"

#include <ctkPluginFrameworkEvent.h>

/**
 * This class registers itself as a listener for framework events and posts them via
 * the ctkEventAdmin as specified in 113.6.3 OSGi R4 compendium.
 */
class ctkEAFrameworkEventAdapter : public QObject, public ctkEAAbstractAdapter
{
  Q_OBJECT

public:

  /**
   * The constructor of the adapter. This will register the adapter with the
   * given context as a <tt>FrameworkListener</tt> and subsequently, will
   * post received events via the given ctkEventAdmin.
   *
   * @param context The plugin context with which to register as a listener.
   * @param admin The <tt>ctkEventAdmin</tt> to use for posting events.
   */
  ctkEAFrameworkEventAdapter(ctkPluginContext* context, ctkEventAdmin* admin);

  void destroy(ctkPluginContext* context);

public Q_SLOTS:

  /**
   * Once a framework event is received this method assembles and posts an event
   * via the <tt>ctkEventAdmin</tt> as specified in 113.6.3 OSGi R4 compendium.
   *
   * @param event The event to adapt.
   */
  void frameworkEvent(const ctkPluginFrameworkEvent& event);
};

#endif // CTKEAFRAMEWORKEVENTADAPTER_P_H
