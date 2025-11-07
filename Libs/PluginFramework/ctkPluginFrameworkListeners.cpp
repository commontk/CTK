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

#include "ctkPluginFrameworkListeners_p.h"

#include "ctkException.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginConstants.h"
#include "ctkLDAPExpr_p.h"
#include "ctkServiceReference_p.h"

#include <QStringList>
#include <QDebug>

const int ctkPluginFrameworkListeners::OBJECTCLASS_IX = 0;
const int ctkPluginFrameworkListeners::SERVICE_ID_IX = 1;
const int ctkPluginFrameworkListeners::SERVICE_PID_IX = 2;

//----------------------------------------------------------------------------
ctkPluginFrameworkListeners::ctkPluginFrameworkListeners(ctkPluginFrameworkContext* pluginFw)
  : pluginFw(pluginFw)
{
  hashedServiceKeys << ctkPluginConstants::OBJECTCLASS.toLower()
      << ctkPluginConstants::SERVICE_ID.toLower()
      << ctkPluginConstants::SERVICE_PID.toLower();

  for (int i = 0; i < hashedServiceKeys.size(); ++i)
  {
    cache.push_back(QHash<QString, QList<ctkServiceSlotEntry> >());
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::addServiceSlot(
    QSharedPointer<ctkPlugin> plugin, QObject* receiver,
    const char* slot, const QString& filter)
{
  QMutexLocker lock(&mutex); Q_UNUSED(lock)
  ctkServiceSlotEntry sse(plugin, receiver, slot, filter);
  if (serviceSet.contains(sse))
  {
    removeServiceSlot_unlocked(plugin, receiver, slot);
  }
  serviceSet.insert(sse);
  checkSimple(sse);

  connect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(serviceListenerDestroyed(QObject*)), Qt::DirectConnection);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::removeServiceSlot(QSharedPointer<ctkPlugin> plugin,
                                                    QObject* receiver,
                                                    const char* slot)
{
  QMutexLocker lock(&mutex);
  removeServiceSlot_unlocked(plugin, receiver, slot);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::removeServiceSlot_unlocked(QSharedPointer<ctkPlugin> plugin,
                                                             QObject* receiver,
                                                             const char* slot)
{
  ctkServiceSlotEntry entryToRemove(plugin, receiver, slot);
  QMutableSetIterator<ctkServiceSlotEntry> it(serviceSet);
  while (it.hasNext())
  {
    ctkServiceSlotEntry currentEntry = it.next();
    if (currentEntry == entryToRemove)
    {
      currentEntry.setRemoved(true);
      //listeners.framework.hooks.handleServiceListenerUnreg(sle);
      removeFromCache(currentEntry);
      it.remove();
      if (slot) break;
    }
  }

  if (plugin)
  {
    disconnect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(serviceListenerDestroyed(QObject*)));
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::serviceListenerDestroyed(QObject *listener)
{
  this->removeServiceSlot(QSharedPointer<ctkPlugin>(0), listener, 0);
}

//----------------------------------------------------------------------------
QSet<ctkServiceSlotEntry> ctkPluginFrameworkListeners::getMatchingServiceSlots(
    const ctkServiceReference& sr, bool lockProps)
{
  QMutexLocker lock(&mutex); Q_UNUSED(lock);

  QSet<ctkServiceSlotEntry> set;
  set.reserve(serviceSet.size());
  // Check complicated or empty listener filters
  int n = 0;
  ctkLDAPExpr expr;
  foreach (const ctkServiceSlotEntry& sse, complicatedListeners)
  {
    ++n;
    expr = sse.getLDAPExpr();
    if (expr.isNull() || expr.evaluate(sr.d_func()->getProperties(), false))
    {
      set.insert(sse);
    }
  }

  if (pluginFw->debug.ldap)
  {
    qDebug() << "Added" << set.size() << "out of" << n
      << "listeners with complicated filters";
  }

  // Check the cache
  QStringList c = sr.d_func()->getProperty(ctkPluginConstants::OBJECTCLASS, lockProps).toStringList();
  foreach (QString objClass, c)
  {
    addToSet(set, OBJECTCLASS_IX, objClass);
  }

  bool ok = false;
  qlonglong service_id = sr.d_func()->getProperty(ctkPluginConstants::SERVICE_ID, lockProps).toLongLong(&ok);
  if (ok)
  {
    addToSet(set, SERVICE_ID_IX, QString::number(service_id));
  }

  QStringList service_pids = sr.d_func()->getProperty(ctkPluginConstants::SERVICE_PID, lockProps).toStringList();
  foreach (QString service_pid, service_pids)
  {
    addToSet(set, SERVICE_PID_IX, service_pid);
  }

  return set;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::frameworkError(QSharedPointer<ctkPlugin> p, const ctkException& e)
{
  emit frameworkEvent(ctkPluginFrameworkEvent(ctkPluginFrameworkEvent::PLUGIN_ERROR, p, e));
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::emitFrameworkEvent(const ctkPluginFrameworkEvent& event)
{
  emit frameworkEvent(event);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::emitPluginChanged(const ctkPluginEvent& event)
{
  emit pluginChangedDirect(event);

  if (!(event.getType() == ctkPluginEvent::STARTING ||
      event.getType() == ctkPluginEvent::STOPPING ||
      event.getType() == ctkPluginEvent::LAZY_ACTIVATION))
  {
    emit pluginChangedQueued(event);
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::serviceChanged(
    const QSet<ctkServiceSlotEntry>& receivers,
    const ctkServiceEvent& evt)
{
  QSet<ctkServiceSlotEntry> matchBefore;
  serviceChanged(receivers, evt, matchBefore);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::serviceChanged(
    const QSet<ctkServiceSlotEntry>& receivers,
    const ctkServiceEvent& evt,
    QSet<ctkServiceSlotEntry>& matchBefore)
{
  ctkServiceReference sr = evt.getServiceReference();
  //QStringList classes = sr.getProperty(ctkPluginConstants::OBJECTCLASS).toStringList();
  int n = 0;

  //framework.hooks.filterServiceEventReceivers(evt, receivers);

  foreach (ctkServiceSlotEntry l, receivers)
  {
    if (!matchBefore.isEmpty())
    {
      matchBefore.remove(l);
    }

    // TODO permission checks
    //if (l.bundle.hasPermission(new ServicePermission(sr, ServicePermission.GET))) {
    //foreach (QString clazz, classes)
    //{
    try
    {
      ++n;
      l.invokeSlot(evt);
    }
    catch (const ctkException& pe)
    {
      frameworkError(l.getPlugin(), pe);
    }
    catch (const std::exception& e)
    {
      frameworkError(l.getPlugin(), ctkRuntimeException(e.what()));
    }

    //break;
    //}
    //}
  }

  if (pluginFw->debug.ldap)
  {
    qDebug() << "Notified" << n << " listeners";
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::removeFromCache(const ctkServiceSlotEntry& sse)
{
  if (!sse.getLocalCache().isEmpty())
  {
    for (int i = 0; i < hashedServiceKeys.size(); ++i)
    {
      QHash<QString, QList<ctkServiceSlotEntry> >& keymap = cache[i];
      QStringList& l = sse.getLocalCache()[i];
      QStringListIterator it(l);
      while (it.hasNext())
      {
        QString value = it.next();
        QList<ctkServiceSlotEntry>& sses = keymap[value];
        sses.removeAll(sse);
        if (sses.isEmpty())
        {
          keymap.remove(value);
        }
      }
    }
  }
  else
  {
    complicatedListeners.removeAll(sse);
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::checkSimple(const ctkServiceSlotEntry& sse)
{
  if (sse.getLDAPExpr().isNull()) // || listeners.nocacheldap) {
  {
    complicatedListeners.push_back(sse);
  }
  else
  {
    ctkLDAPExpr::LocalCache local_cache;
    if (sse.getLDAPExpr().isSimple(hashedServiceKeys, local_cache, false))
    {
      sse.getLocalCache() = local_cache;
      for (int i = 0; i < hashedServiceKeys.size(); ++i)
      {
        QStringListIterator it(local_cache[i]);
        while (it.hasNext())
        {
          QString value = it.next();
          QList<ctkServiceSlotEntry>& sses = cache[i][value];
          sses.push_back(sse);
        }
      }
    }
    else
    {
      if (pluginFw->debug.ldap)
      {
        qDebug() << "## DEBUG: Too complicated filter:" << sse.getFilter();
      }
      complicatedListeners.push_back(sse);
    }
  }
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkListeners::addToSet(QSet<ctkServiceSlotEntry>& set,
                                           int cache_ix, const QString& val)
{
  QList<ctkServiceSlotEntry>& l = cache[cache_ix][val];
  if (!l.isEmpty())
  {
    if (pluginFw->debug.ldap)
    {
      qDebug() << hashedServiceKeys[cache_ix] << "matches" << l.size();
    }
    foreach (ctkServiceSlotEntry entry, l)
    {
      set.insert(entry);
    }
  }
  else
  {
    if (pluginFw->debug.ldap)
    {
      qDebug() << hashedServiceKeys[cache_ix] << "matches none";
    }
  }
}
