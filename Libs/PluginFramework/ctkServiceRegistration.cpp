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

#include <QMutex>

#include "ctkPluginFrameworkContext_p.h"
#include "ctkPlugin_p.h"
#include "ctkPluginConstants.h"
#include "ctkServiceFactory.h"
#include "ctkServiceRegistration.h"
#include "ctkServiceRegistration_p.h"
#include "ctkServices_p.h"
#include "ctkServiceSlotEntry_p.h"

#include <stdexcept>

//----------------------------------------------------------------------------
ctkServiceRegistration::ctkServiceRegistration()
  : d_ptr(0)
{

}

//----------------------------------------------------------------------------
ctkServiceRegistration::ctkServiceRegistration(const ctkServiceRegistration& reg)
  : d_ptr(reg.d_ptr)
{
  if (d_func()) d_func()->ref.ref();
}

//----------------------------------------------------------------------------
ctkServiceRegistration::ctkServiceRegistration(ctkServiceRegistrationPrivate* registrationPrivate)
  : d_ptr(registrationPrivate)
{
  if(d_func()) d_func()->ref.ref();
}

//----------------------------------------------------------------------------
ctkServiceRegistration::ctkServiceRegistration(ctkPluginPrivate* plugin, QObject* service,
                    const ctkDictionary& props)
  : d_ptr(new ctkServiceRegistrationPrivate(plugin, service, props))
{

}

//----------------------------------------------------------------------------
ctkServiceRegistration::operator bool() const
{
  return d_func();
}

//----------------------------------------------------------------------------
ctkServiceRegistration& ctkServiceRegistration::operator=(int null)
{
  if (null == 0)
  {
    if (d_func() && !d_func()->ref.deref())
    {
      delete d_ptr;
    }
    d_ptr = 0;
  }
  return *this;
}

//----------------------------------------------------------------------------
ctkServiceRegistration::~ctkServiceRegistration()
{
  if (d_func() && !d_func()->ref.deref())
    delete d_ptr;
}

//----------------------------------------------------------------------------
ctkServiceReference ctkServiceRegistration::getReference() const
{
  Q_D(const ctkServiceRegistration);

  if (!d) throw ctkIllegalStateException("ctkServiceRegistration object invalid");
  if (!d->available) throw ctkIllegalStateException("Service is unregistered");

  return d->reference;
}

//----------------------------------------------------------------------------
void ctkServiceRegistration::setProperties(const ctkDictionary& props)
{
  Q_D(ctkServiceRegistration);
  if (!d) throw ctkIllegalStateException("ctkServiceRegistration object invalid");

  QMutexLocker lock(&d->eventLock);

  QSet<ctkServiceSlotEntry> before;
  // TBD, optimize the locking of services
  {
    QMutexLocker lock2(&d->plugin->fwCtx->globalFwLock);
    QMutexLocker lock3(&d->propsLock);

    if (d->available)
    {
      // NYI! Optimize the MODIFIED_ENDMATCH code
      int old_rank = d->properties.value(ctkPluginConstants::SERVICE_RANKING).toInt();
      before = d->plugin->fwCtx->listeners.getMatchingServiceSlots(d->reference, false);
      QStringList classes = d->properties.value(ctkPluginConstants::OBJECTCLASS).toStringList();
      qlonglong sid = d->properties.value(ctkPluginConstants::SERVICE_ID).toLongLong();
      d->properties = ctkServices::createServiceProperties(props, classes, sid);
      int new_rank = d->properties.value(ctkPluginConstants::SERVICE_RANKING).toInt();
      if (old_rank != new_rank)
      {
        d->plugin->fwCtx->services->updateServiceRegistrationOrder(*this, classes);
      }
    }
    else
    {
      throw ctkIllegalStateException("Service is unregistered");
    }
  }
  d->plugin->fwCtx->listeners.serviceChanged(
      d->plugin->fwCtx->listeners.getMatchingServiceSlots(d->reference),
      ctkServiceEvent(ctkServiceEvent::MODIFIED, d->reference), before);

  d->plugin->fwCtx->listeners.serviceChanged(
      before,
      ctkServiceEvent(ctkServiceEvent::MODIFIED_ENDMATCH, d->reference));
}

//----------------------------------------------------------------------------
void ctkServiceRegistration::unregister()
{
  Q_D(ctkServiceRegistration);
  if (!d) throw ctkIllegalStateException("ctkServiceRegistration object invalid");

  if (d->unregistering) return; // Silently ignore redundant unregistration.
  {
    QMutexLocker lock(&d->eventLock);
    if (d->unregistering) return;
    d->unregistering = true;

    if (d->available)
    {
      if (d->plugin)
      {
        d->plugin->fwCtx->services->removeServiceRegistration(*this);
      }
    }
    else
    {
      throw ctkIllegalStateException("Service is unregistered");
    }
  }

  if (d->plugin)
  {
     d->plugin->fwCtx->listeners.serviceChanged(
         d->plugin->fwCtx->listeners.getMatchingServiceSlots(d->reference),
         ctkServiceEvent(ctkServiceEvent::UNREGISTERING, d->reference));
  }

  {
    QMutexLocker lock(&d->eventLock);
    {
      QMutexLocker lock2(&d->propsLock);
      d->available = false;
      if (d->plugin)
      {
        for (QHashIterator<QSharedPointer<ctkPlugin>, QObject*> i(d->serviceInstances); i.hasNext();)
        {
          QObject* obj = i.next().value();
          try
          {
            // NYI, don't call inside lock
            qobject_cast<ctkServiceFactory*>(d->service)->ungetService(i.key(),
                                                                       *this,
                                                                       obj);
          }
          catch (const ctkException& ue)
          {
            ctkPluginFrameworkEvent pfwEvent(ctkPluginFrameworkEvent::PLUGIN_ERROR, d->plugin->q_func(), ue);
            d->plugin->fwCtx->listeners.emitFrameworkEvent(pfwEvent);
          }
        }
      }
      d->plugin = 0;
      d->dependents.clear();
      d->service = 0;
      d->serviceInstances.clear();
      d->reference = 0;
      d->unregistering = false;
    }
  }
}

//----------------------------------------------------------------------------
bool ctkServiceRegistration::operator<(const ctkServiceRegistration& o) const
{
  Q_D(const ctkServiceRegistration);
  if (!d) return true;
  return d->reference <(o.d_func()->reference);
}

//----------------------------------------------------------------------------
bool ctkServiceRegistration::operator==(const ctkServiceRegistration& registration) const
{
  Q_D(const ctkServiceRegistration);
  return d == registration.d_func();
}

//----------------------------------------------------------------------------
ctkServiceRegistration& ctkServiceRegistration::operator=(const ctkServiceRegistration& registration)
{
  ctkServiceRegistrationPrivate* curr_d = d_func();
  d_ptr = registration.d_ptr;
  if (d_ptr) d_ptr->ref.ref();

  if (curr_d && !curr_d->ref.deref())
    delete curr_d;

  return *this;
}

//----------------------------------------------------------------------------
uint qHash(const ctkServiceRegistration& serviceReg)
{
  return qHash(serviceReg.d_func());
}
