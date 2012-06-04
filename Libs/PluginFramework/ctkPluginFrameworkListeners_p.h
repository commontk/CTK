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

#ifndef CTKPLUGINFRAMEWORKLISTENERS_H
#define CTKPLUGINFRAMEWORKLISTENERS_H

#include <QObject>
#include <QHash>
#include <QSet>
#include <QMutex>

#include "ctkPluginEvent.h"
#include "ctkPluginFrameworkEvent.h"
#include "ctkServiceReference.h"
#include "ctkServiceSlotEntry_p.h"
#include "ctkServiceEvent.h"

/**
 * \ingroup PluginFramework
 */
class ctkPluginFrameworkListeners : public QObject
{

  Q_OBJECT

public:

  ctkPluginFrameworkListeners(ctkPluginFrameworkContext* pluginFw);

  /**
   * Add a slot receiving service envents with filter to the current framework.
   * If no filter is wanted, call with a null filter.
   *
   * @param plugin Who wants to add the slot.
   * @param listener Object to add.
   * @param filter LDAP String used for filtering event before calling listener.
   */
  void addServiceSlot(QSharedPointer<ctkPlugin> plugin, QObject* receiver,
                      const char* slot, const QString& filter);

  /**
   * Remove a slot connected to service events.
   *
   * @param plugin The plugin removing this listener.
   * @param receiver The receiver containing the slot.
   * @param slot The slot in the receiver.
   */
  void removeServiceSlot(QSharedPointer<ctkPlugin> plugin, QObject* receiver,
                         const char* slot);

  /**
   * Gets the slots interested in modifications of the service reference
   *
   * @param sr The reference related to the event describing the service modification.
   * @param lockProps If access to the properties of the service object referenced by sr
   *        should be synchronized.
   * @return A set of listeners to notify.
   */
  QSet<ctkServiceSlotEntry> getMatchingServiceSlots(const ctkServiceReference& sr, bool lockProps = true);

  /**
   * Convenience method for throwing framework error event.
   *
   * @param p Plugin which caused the error.
   * @param e The exception.
   */
  void frameworkError(QSharedPointer<ctkPlugin> p, const ctkException& e);

  /**
   * Receive notification that a service has had a change occur in its lifecycle.
   */
  void serviceChanged(const QSet<ctkServiceSlotEntry>& receivers,
                      const ctkServiceEvent& evt,
                      QSet<ctkServiceSlotEntry>& matchBefore);

  void serviceChanged(const QSet<ctkServiceSlotEntry>& receivers,
                      const ctkServiceEvent& evt);

  void emitPluginChanged(const ctkPluginEvent& event);

  void emitFrameworkEvent(const ctkPluginFrameworkEvent& event);

Q_SIGNALS:

  void pluginChangedDirect(const ctkPluginEvent& event);
  void pluginChangedQueued(const ctkPluginEvent& event);

  void frameworkEvent(const ctkPluginFrameworkEvent& event);

private Q_SLOTS:

  void serviceListenerDestroyed(QObject* listener);

private:

  QMutex mutex;

  QList<QString> hashedServiceKeys;
  static const int OBJECTCLASS_IX; // = 0;
  static const int SERVICE_ID_IX; // = 1;
  static const int SERVICE_PID_IX; // = 2;

  // Service listeners with complicated or empty filters
  QList<ctkServiceSlotEntry> complicatedListeners;

  // Service listeners with "simple" filters are cached
  QList<QHash<QString, QList<ctkServiceSlotEntry> > > cache;

  QSet<ctkServiceSlotEntry> serviceSet;

  ctkPluginFrameworkContext* pluginFw;

  /**
   * Remove all references to a service slot from the service listener
   * cache.
   */
  void removeFromCache(const ctkServiceSlotEntry& sse);

  /**
   * Checks if the specified service slot's filter is simple enough
   * to cache.
   */
  void checkSimple(const ctkServiceSlotEntry& sse);

  /**
   * Add all members of the specified list to the specified set.
   */
  void addToSet(QSet<ctkServiceSlotEntry>& set, int cache_ix, const QString& val);

  /**
   * The unsynchronized version of removeServiceSlot().
   */
  void removeServiceSlot_unlocked(QSharedPointer<ctkPlugin> plugin, QObject* receiver,
                                  const char* slot);
};


#endif // CTKPLUGINFRAMEWORKLISTENERS_H
