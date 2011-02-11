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


#ifndef CTKCONFIGURATIONLISTENER_H
#define CTKCONFIGURATIONLISTENER_H

#include "ctkConfigurationEvent.h"

/**
 * \ingroup ConfigAdmin
 *
 * Listener for Configuration Events. When a <code>ctkConfigurationEvent</code>
 * is fired, it is asynchronously delivered to a
 * <code>ctkConfigurationListener</code>.
 *
 * <p>
 * <code>ctkConfigurationListener</code> objects are registered with the
 * Framework service registry and are notified with a
 * <code>ctkConfigurationEvent</code> object when an event is fired.
 * <p>
 * <code>ctkConfigurationListener</code> objects can inspect the received
 * <code>ctkConfigurationEvent</code> object to determine its type, the pid of
 * the <code>ctkConfiguration</code> object with which it is associated, and the
 * Configuration Admin service that fired the event.
 *
 * <p>
 * Security Considerations. Plugins wishing to monitor configuration events will
 * require <code>ctkServicePermission[ctkConfigurationListener,REGISTER]</code> to
 * register a <code>ctkConfigurationListener</code> service.
 */
struct ctkConfigurationListener
{
  virtual ~ctkConfigurationListener() {}

  /**
   * Receives notification of a ctkConfiguration that has changed.
   *
   * @param event The <code>ctkConfigurationEvent</code>.
   */
  virtual void configurationEvent(const ctkConfigurationEvent& event) = 0;
};

Q_DECLARE_INTERFACE(ctkConfigurationListener, "org.commontk.service.cm.ConfigurationListener")

#endif // CTKCONFIGURATIONLISTENER_H
