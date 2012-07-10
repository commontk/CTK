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

#ifndef CTKEVENTADMINBUS_H
#define CTKEVENTADMINBUS_H

#include <service/event/ctkEventAdmin.h>


/**
 * The Event Admin service. Plugins wishing to publish events can either
 * obtain the Event Admin service and call one of the event delivery methods
 * or publish a Qt signal for a specific event topic.
 *
 */
class ctkEventAdminBus : public ctkEventAdmin
{
public:
  /// @@@ WRITE DOC!
  virtual bool createServer(const QString &communication_protocol, unsigned int listen_port) = 0;

  /// @@@ WRITE DOC!
  virtual void startListen() = 0;

  /// @@@ WRITE DOC!
  virtual bool createClient(const QString &communication_protocol, const QString &server_host, unsigned int port) = 0;

};


Q_DECLARE_INTERFACE(ctkEventAdminBus, "org.commontk.service.event.EventAdminBus")

#endif // CTKEVENTADMIN_H
