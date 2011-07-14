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

#ifndef CTKEVENTBUSIMPL_H
#define CTKEVENTBUSIMPL_H

#include <EventBus/ctkEventBus.h>

#include <QList>
#include <QHash>
#include <QSet>

class ctkEventHandlerWrapper;

class ctkEventBusImpl : public QObject,
                     public ctkEventBus
{
  Q_OBJECT
  Q_INTERFACES(ctkEventBus)

public:

  static ctkEventBusImpl* instance();

  void postEvent(const ctkEvent& event);
  void sendEvent(const ctkEvent& event);

  void publishSignal(const QObject* publisher, const char* signal, const QString& topic, Qt::ConnectionType type = Qt::QueuedConnection);

  QString subscribeSlot(const QObject* subscriber, const char* member, const ctkProperties& properties);

  void updateProperties(const QString& subscriptionId, const ctkProperties& properties);

protected:

  typedef QList<ctkEventHandlerWrapper*> HandlerList;

  HandlerList globalWildcard;

  QHash<QString, HandlerList> topicName;

  void dispatchEvent(const ctkEvent& event, bool isAsync);

  void bucket(ctkEventHandlerWrapper* wrapper);

  QSet<ctkEventHandlerWrapper*> handlers(const QString& topic);

private:

  ctkEventBusImpl();
};

#endif // CTKEVENTBUSIMPL_H
