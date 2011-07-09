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

#ifndef CTKEVENTHANDLERWRAPPER_P_H
#define CTKEVENTHANDLERWRAPPER_P_H

#include <QStringList>

#include <EventBus/ctkEventBus.h>
#include <EventBus/ctkEventConstants.h>
#include <ctkLDAPSearchFilter.h>

#include <iostream>

class ctkEventHandlerWrapper : public QObject {

  Q_OBJECT

private:

  ctkProperties properties;
  QStringList topicList;
  ctkLDAPSearchFilter filter;

public:

  ctkEventHandlerWrapper(const QObject* subscriber, const char* handler, const ctkProperties& properties)
    : properties(properties)
  {
    connect(this, SIGNAL(notifySubscriber(Event)), subscriber, handler);
  }

  QStringList topics() const
  {
    return topicList;
  }

  bool init()
  {
    topicList.clear();

    // Get topic names
    QVariant v = properties[EventConstants::EVENT_TOPIC];
    topicList = v.toStringList();

    if (topicList.empty())
    {
      return false;
    }

    v = properties[EventConstants::EVENT_FILTER];
    filter = ctkLDAPSearchFilter(v.toString());
    return true;
  }

  void handleEvent(const ctkEvent& event /*, const Permission& perm */)
  {
    if (!event.matches(filter)) return;

    // should do permissions checks now somehow
    // ...

    try {
      emit notifySubscriber(event);
    }
    catch (const std::exception& e)
    {
      // TODO logging
      std::cerr << "Exception occured during publishing " << qPrintable(event.topic()) << ": " << e.what() << std::endl;
    }

  }

signals:

  void notifySubscriber(const ctkEvent&);

};

#endif // CTKEVENTHANDLERWRAPPER_P_H
