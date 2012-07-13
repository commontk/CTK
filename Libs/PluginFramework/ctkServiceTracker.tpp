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


#include "ctkServiceTracker_p.h"
#include "ctkTrackedService_p.h"
#include "ctkServiceException.h"
#include "ctkPluginConstants.h"
#include "ctkPluginContext.h"

#include <QVarLengthArray>
#include <QDebug>

#include <stdexcept>
#include <limits>

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTracker<S,T>::~ctkServiceTracker()
{
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTracker<S,T>::ctkServiceTracker(ctkPluginContext* context,
                                          const ctkServiceReference& reference,
                                          ServiceTrackerCustomizer* customizer)
  : d_ptr(new ServiceTrackerPrivate(this, context, reference, customizer))
{
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTracker<S,T>::ctkServiceTracker(ctkPluginContext* context, const QString& clazz,
                                          ServiceTrackerCustomizer* customizer)
  : d_ptr(new ServiceTrackerPrivate(this, context, clazz, customizer))
{
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTracker<S,T>::ctkServiceTracker(ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
                                          ServiceTrackerCustomizer* customizer)
  : d_ptr(new ServiceTrackerPrivate(this, context, filter, customizer))
{
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceTracker<S,T>::ctkServiceTracker(ctkPluginContext *context, ctkServiceTrackerCustomizer<T> *customizer)
  : d_ptr(new ServiceTrackerPrivate(this, context, qobject_interface_iid<S>(), customizer))
{
  const char* clazz = qobject_interface_iid<S>();
  if (clazz == 0) throw ctkServiceException("The service interface class has no Q_DECLARE_INTERFACE macro");
}

//----------------------------------------------------------------------------
template<class S, class T>
void ctkServiceTracker<S,T>::open()
{
  Q_D(ServiceTracker);
  QSharedPointer<TrackedService> t;
  {
    QMutexLocker lock(&d->mutex);
    if (d->trackedService)
    {
      return;
    }

    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker<S,T>::open: " << d->filter;
    }

    t = QSharedPointer<TrackedService>(
          new TrackedService(this, d->customizer));
    {
      QMutexLocker lockT(t.data());
      try {
        d->context->connectServiceListener(t.data(), "serviceChanged", d->listenerFilter);
        QList<ctkServiceReference> references;
        if (!d->trackClass.isEmpty())
        {
          references = d->getInitialReferences(d->trackClass, QString());
        }
        else
        {
          if (!d->trackReference.getPlugin().isNull())
          {
            references.push_back(d->trackReference);
          }
          else
          { /* user supplied filter */
            references = d->getInitialReferences(QString(),
                                              (d->listenerFilter.isNull()) ? d->filter.toString() : d->listenerFilter);
          }
        }
        /* set tracked with the initial references */
        t->setInitial(references);
      }
      catch (const ctkInvalidArgumentException& e)
      {
        throw ctkRuntimeException(QString("unexpected ctkInvalidArgumentException exception: %1").arg(e.what()));
      }
    }
    d->trackedService = t;
  }
  /* Call tracked outside of synchronized region */
  t->trackInitial(); /* process the initial references */
}

//----------------------------------------------------------------------------
template<class S, class T>
void ctkServiceTracker<S,T>::close()
{
  Q_D(ServiceTracker);
  QSharedPointer<TrackedService> outgoing;
  QList<ctkServiceReference> references;
  {
    QMutexLocker lock(&d->mutex);
    outgoing = d->trackedService;
    if (outgoing.isNull())
    {
      return;
    }
    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker<S,T>::close:" << d->filter;
    }
    outgoing->close();
    references = getServiceReferences();
    d->trackedService.clear();;
    try
    {
      d->context->disconnectServiceListener(outgoing.data(), "serviceChanged");
    }
    catch (const ctkIllegalStateException& /*e*/)
    {
      /* In case the context was stopped. */
    }
  }
  d->modified(); /* clear the cache */
  {
    QMutexLocker lockT(outgoing.data());
    outgoing->wakeAll(); /* wake up any waiters */
  }
  foreach (ctkServiceReference ref, references)
  {
    outgoing->untrack(ref, ctkServiceEvent());
  }

  if (d->DEBUG)
  {
    QMutexLocker lock(&d->mutex);
    if ((d->cachedReference.getPlugin().isNull()) && (d->cachedService == 0))
    {
      qDebug() << "ctkServiceTracker<S,T>::close[cached cleared]:"
          << d->filter;
    }
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
T ctkServiceTracker<S,T>::waitForService(unsigned long timeout)
{
  Q_D(ServiceTracker);
  T object = getService();
  while (object == 0)
  {
    QSharedPointer<TrackedService> t = d->tracked();
    if (t.isNull())
    { /* if ServiceTracker is not open */
      return 0;
    }
    {
      QMutexLocker lockT(t.data());
      if (t->size() == 0)
      {
        t->wait(timeout);
      }
    }
    object = getService();
    if (timeout > 0)
    {
      return object;
    }
  }
  return object;
}

//----------------------------------------------------------------------------
template<class S, class T>
QList<ctkServiceReference> ctkServiceTracker<S,T>::getServiceReferences() const
{
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return QList<ctkServiceReference>();
  }
  {
    QMutexLocker lockT(t.data());
    return d->getServiceReferences_unlocked(t.data());
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
ctkServiceReference ctkServiceTracker<S,T>::getServiceReference() const
{
  Q_D(const ServiceTracker);
  ctkServiceReference reference(0);
  {
    QMutexLocker lock(&d->mutex);
    reference = d->cachedReference;
  }
  if (!reference.getPlugin().isNull())
  {
    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker<S,T>::getServiceReference[cached]:"
                   << d->filter;
    }
    return reference;
  }
  if (d->DEBUG)
  {
    qDebug() << "ctkServiceTracker<S,T>::getServiceReference:" << d->filter;
  }
  QList<ctkServiceReference> references = getServiceReferences();
  int length = references.size();
  if (length == 0)
  { /* if no service is being tracked */
    throw ctkServiceException("No service is being tracked");
  }
  int index = 0;
  if (length > 1)
  { /* if more than one service, select highest ranking */
    QVarLengthArray<int, 10> rankings(length);
    int count = 0;
    int maxRanking = std::numeric_limits<int>::min();
    for (int i = 0; i < length; i++)
    {
      bool ok = false;
      int ranking = references[i].getProperty(ctkPluginConstants::SERVICE_RANKING).toInt(&ok);
      if (!ok) ranking = 0;

      rankings[i] = ranking;
      if (ranking > maxRanking)
      {
        index = i;
        maxRanking = ranking;
        count = 1;
      }
      else
      {
        if (ranking == maxRanking)
        {
          count++;
        }
      }
    }
    if (count > 1)
    { /* if still more than one service, select lowest id */
      qlonglong minId = std::numeric_limits<qlonglong>::max();
      for (int i = 0; i < length; i++)
      {
        if (rankings[i] == maxRanking)
        {
          qlonglong id = references[i].getProperty(ctkPluginConstants::SERVICE_ID).toLongLong();
          if (id < minId)
          {
            index = i;
            minId = id;
          }
        }
      }
    }
  }

  {
    QMutexLocker lock(&d->mutex);
    d->cachedReference = references[index];
    return d->cachedReference;
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
T ctkServiceTracker<S,T>::getService(const ctkServiceReference& reference) const
{
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return 0;
  }
  {
    QMutexLocker lockT(t.data());
    return t->getCustomizedObject(reference);
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
QList<T> ctkServiceTracker<S,T>::getServices() const
{
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return QList<T>();
  }
  {
    QMutexLocker lockT(t.data());
    QList<ctkServiceReference> references = d->getServiceReferences_unlocked(t.data());
    QList<T> objects;
    foreach (ctkServiceReference ref, references)
    {
      //objects << getService(ref);
      objects << t->getCustomizedObject(ref);
    }
    return objects;
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
T ctkServiceTracker<S,T>::getService() const
{
  Q_D(const ServiceTracker);
  T service = d->cachedService;
  if (service != 0)
  {
    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker<S,T>::getService[cached]:"
               << d->filter;
    }
    return service;
  }
  if (d->DEBUG)
  {
    qDebug() << "ctkServiceTracker<S,T>::getService:" << d->filter;
  }

  try
  {
    ctkServiceReference reference = getServiceReference();
    if (reference.getPlugin().isNull())
    {
      return 0;
    }
    return d->cachedService = getService(reference);
  }
  catch (const ctkServiceException&)
  {
    return 0;
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
void ctkServiceTracker<S,T>::remove(const ctkServiceReference& reference)
{
  Q_D(ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return;
  }
  t->untrack(reference, ctkServiceEvent());
}

//----------------------------------------------------------------------------
template<class S, class T>
int ctkServiceTracker<S,T>::size() const
{
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return 0;
  }
  {
    QMutexLocker lockT(t.data());
    return t->size();
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
int ctkServiceTracker<S,T>::getTrackingCount() const
{
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return -1;
  }
  {
    QMutexLocker lockT(t.data());
    return t->getTrackingCount();
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
QMap<ctkServiceReference, T> ctkServiceTracker<S,T>::getTracked() const
{
  QMap<ctkServiceReference, T> map;
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return map;
  }
  {
    QMutexLocker lockT(t.data());
    return t->copyEntries(map);
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
bool ctkServiceTracker<S,T>::isEmpty() const
{
  Q_D(const ServiceTracker);
  QSharedPointer<TrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return true;
  }
  {
    QMutexLocker lockT(t.data());
    return t->isEmpty();
  }
}

//----------------------------------------------------------------------------
template<class S, class T>
T ctkServiceTracker<S,T>::addingService(const ctkServiceReference& reference)
{
  Q_D(ServiceTracker);
  return qobject_cast<T>(d->context->getService(reference));
}

//----------------------------------------------------------------------------
template<class S, class T>
void ctkServiceTracker<S,T>::modifiedService(const ctkServiceReference& reference, T service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)
  /* do nothing */
}

//----------------------------------------------------------------------------
template<class S, class T>
void ctkServiceTracker<S,T>::removedService(const ctkServiceReference& reference, T service)
{
  Q_UNUSED(service)

  Q_D(ServiceTracker);
  d->context->ungetService(reference);
}
