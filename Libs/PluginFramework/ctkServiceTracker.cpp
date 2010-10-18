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


#include "ctkServiceTracker.h"
#include "ctkServiceTrackerPrivate.h"
#include "ctkTrackedService_p.h"
#include "ctkServiceException.h"
#include "ctkPluginConstants.h"

#include <QVarLengthArray>
#include <QDebug>

#include <stdexcept>
#include <limits>


ctkServiceTracker::~ctkServiceTracker()
{
}

ctkServiceTracker::ctkServiceTracker(ctkPluginContext* context,
                  const ctkServiceReference& reference,
                  ctkServiceTrackerCustomizer* customizer)
  : d_ptr(new ctkServiceTrackerPrivate(this, context, reference, customizer))
{
}

ctkServiceTracker::ctkServiceTracker(ctkPluginContext* context, const QString& clazz,
                      ctkServiceTrackerCustomizer* customizer)
  : d_ptr(new ctkServiceTrackerPrivate(this, context, clazz, customizer))
{
}

ctkServiceTracker::ctkServiceTracker(ctkPluginContext* context, const ctkLDAPSearchFilter& filter,
                      ctkServiceTrackerCustomizer* customizer)
  : d_ptr(new ctkServiceTrackerPrivate(this, context, filter, customizer))
{
}

void ctkServiceTracker::open()
{
  Q_D(ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t;
  {
    QMutexLocker lock(&d->mutex);
    if (d->trackedService)
    {
      return;
    }

    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker::open: " << d->filter;
    }

    t = QSharedPointer<ctkTrackedService>(new ctkTrackedService(this, d->customizer));
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
          if (!d->trackReference.isNull())
          {
            if (d->trackReference.getPlugin())
            {
              references.push_back(d->trackReference);
            }
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
      catch (const std::invalid_argument& e)
      {
        throw std::runtime_error(std::string("unexpected std::invalid_argument exception: ")
            + e.what());
      }
    }
    d->trackedService = t;
  }
  /* Call tracked outside of synchronized region */
  t->trackInitial(); /* process the initial references */
}

void ctkServiceTracker::close()
{
  Q_D(ctkServiceTracker);
  QSharedPointer<ctkTrackedService> outgoing;
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
      qDebug() << "ctkServiceTracker::close:" << d->filter;
    }
    outgoing->close();
    references = getServiceReferences();
    d->trackedService.clear();;
    try
    {
      d->context->disconnectServiceListener(outgoing.data(), "serviceChanged");
    }
    catch (const std::logic_error& e)
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
    if ((d->cachedReference.isNull()) && (d->cachedService == 0))
    {
      qDebug() << "ctkServiceTracker::close[cached cleared]:"
          << d->filter;
    }
  }
}

QObject* ctkServiceTracker::waitForService(unsigned long timeout)
{
  Q_D(ctkServiceTracker);
  QObject* object = getService();
  while (object == 0)
  {
    QSharedPointer<ctkTrackedService> t = d->tracked();
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

QList<ctkServiceReference> ctkServiceTracker::getServiceReferences() const
{
  Q_D(const ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return QList<ctkServiceReference>();
  }
  {
    QMutexLocker lockT(t.data());
    if (t->size() == 0)
    {
      return QList<ctkServiceReference>();
    }
    return t->getTracked();
  }
}

ctkServiceReference ctkServiceTracker::getServiceReference() const
{
  Q_D(const ctkServiceTracker);
  ctkServiceReference reference(0);
  {
    QMutexLocker lock(&d->mutex);
    reference = d->cachedReference;
  }
  if (!reference.isNull())
  {
    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker::getServiceReference[cached]:"
                   << d->filter;
    }
    return reference;
  }
  if (d->DEBUG)
  {
    qDebug() << "ctkServiceTracker::getServiceReference:" << d->filter;
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

QObject* ctkServiceTracker::getService(const ctkServiceReference& reference) const
{
  Q_D(const ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return 0;
  }
  {
    QMutexLocker lockT(t.data());
    return t->getCustomizedObject(reference).value<QObject*>();
  }
}

QList<QObject*> ctkServiceTracker::getServices() const
{
  Q_D(const ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return QList<QObject*>();
  }
  {
    QMutexLocker lockT(t.data());
    QList<ctkServiceReference> references = getServiceReferences();
    QList<QObject*> objects;
    foreach (ctkServiceReference ref, references)
    {
      objects << getService(ref);
    }
    return objects;
  }
}

QObject* ctkServiceTracker::getService() const
{
  Q_D(const ctkServiceTracker);
  QObject* service = d->cachedService;
  if (service != 0)
  {
    if (d->DEBUG)
    {
      qDebug() << "ctkServiceTracker::getService[cached]:"
                   << d->filter;
    }
    return service;
  }
  if (d->DEBUG)
  {
    qDebug() << "ctkServiceTracker::getService:" << d->filter;
  }

  try
  {
    ctkServiceReference reference = getServiceReference();
    if (reference.isNull())
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

void ctkServiceTracker::remove(const ctkServiceReference& reference)
{
  Q_D(ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return;
  }
  t->untrack(reference, ctkServiceEvent());
}

int ctkServiceTracker::size() const
{
  Q_D(const ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return 0;
  }
  {
    QMutexLocker lockT(t.data());
    return t->size();
  }
}

int ctkServiceTracker::getTrackingCount() const
{
  Q_D(const ctkServiceTracker);
  QSharedPointer<ctkTrackedService> t = d->tracked();
  if (t.isNull())
  { /* if ServiceTracker is not open */
    return -1;
  }
  {
    QMutexLocker lockT(t.data());
    return t->getTrackingCount();
  }
}

QObject* ctkServiceTracker::addingService(const ctkServiceReference& reference)
{
  Q_D(ctkServiceTracker);
  return d->context->getService(reference);
}

void ctkServiceTracker::modifiedService(const ctkServiceReference& reference, QObject* service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)
  /* do nothing */
}

void ctkServiceTracker::removedService(const ctkServiceReference& reference, QObject* service)
{
  Q_UNUSED(service)

  Q_D(ctkServiceTracker);
  d->context->ungetService(reference);
}
