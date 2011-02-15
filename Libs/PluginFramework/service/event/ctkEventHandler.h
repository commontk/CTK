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


#ifndef CTKEVENTHANDLER_H
#define CTKEVENTHANDLER_H

#include "ctkEvent.h"

/**
 * \ingroup EventAdmin
 *
 * Listener for Events.
 *
 * <p>
 * <code>ctkEventHandler</code> objects are registered with the Framework service
 * registry and are notified with an <code>ctkEvent</code> object when an event
 * is sent or posted.
 * <p>
 * <code>ctkEventHandler</code> objects can inspect the received
 * <code>ctkEvent</code> object to determine its topic and properties.
 *
 * <p>
 * <code>ctkEventHandler</code> objects must be registered with a service
 * property {@link ctkEventConstants#EVENT_TOPIC} whose value is the list of topics
 * in which the event handler is interested.
 * <p>
 * For example:
 *
 * \code
 * QStringList topics("com/isv/&#42;");
 * ctkDictionary props;
 * props.insert(ctkEventConstants::EVENT_TOPIC, topics);
 * context->registerService<ctkEventHandler>(this, props);
 * \endcode
 *
 * Event Handler services can also be registered with an
 * {@link ctkEventConstants#EVENT_FILTER} service property to further filter the
 * events. If the syntax of this filter is invalid, then the Event Handler must
 * be ignored by the Event Admin service. The Event Admin service should log a
 * warning.
 * <p>
 * Security Considerations. Plugins wishing to monitor <code>ctkEvent</code>
 * objects will require <code>ctkServicePermission[ctkEventHandler,REGISTER]</code>
 * to register a <code>ctkEventHandler</code> service. The plugin must also have
 * <code>ctkTopicPermission[topic,SUBSCRIBE]</code> for the topic specified in
 * the event in order to receive the event.
 *
 * @see ctkEvent
 *
 * @remarks This class is thread safe.
 */
struct ctkEventHandler
{
  virtual ~ctkEventHandler() {}

  /**
   * Called by the {@link ctkEventAdmin} service to notify the listener of an
   * event.
   *
   * @param event The event that occurred.
   */
  virtual void handleEvent(const ctkEvent& event) = 0;
};

Q_DECLARE_INTERFACE(ctkEventHandler, "org.commontk.service.event.EventHandler")

#endif // CTKEVENTHANDLER_H
