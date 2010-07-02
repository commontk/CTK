/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkServiceRegistration.h"
#include "ctkServiceRegistrationPrivate.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginPrivate_p.h"
#include "ctkServiceFactory.h"

#include <QMutex>

#include <stdexcept>


  ctkServiceRegistration::ctkServiceRegistration(ctkPluginPrivate* plugin, QObject* service,
                      const ServiceProperties& props)
    : d_ptr(new ctkServiceRegistrationPrivate(this, plugin, service, props))
  {

  }

  ctkServiceRegistration::ctkServiceRegistration(ctkServiceRegistrationPrivate& dd)
    : d_ptr(&dd)
  {

  }

  ctkServiceReference* ctkServiceRegistration::getReference()
  {
    Q_D(ctkServiceRegistration);

    if (!d->available) throw std::logic_error("Service is unregistered");

    return d->reference;
  }

  void ctkServiceRegistration::setProperties(const ServiceProperties& properties)
  {
    Q_UNUSED(properties)
//    QMutexLocker lock(eventLock);
//          Set before;
//          // TBD, optimize the locking of services
//          synchronized (bundle.fwCtx.services) {
//
//            synchronized (properties) {
//              if (available) {
//                // NYI! Optimize the MODIFIED_ENDMATCH code
//                Object old_rank = properties.get(Constants.SERVICE_RANKING);
//                before = bundle.fwCtx.listeners.getMatchingServiceListeners(reference);
//                String[] classes = (String[])properties.get(Constants.OBJECTCLASS);
//                Long sid = (Long)properties.get(Constants.SERVICE_ID);
//                properties = new PropertiesDictionary(props, classes, sid);
//                Object new_rank = properties.get(Constants.SERVICE_RANKING);
//                if (old_rank != new_rank && new_rank instanceof Integer &&
//                    !((Integer)new_rank).equals(old_rank)) {
//                  bundle.fwCtx.services.updateServiceRegistrationOrder(this, classes);
//                }
//              } else {
//                throw new IllegalStateException("Service is unregistered");
//              }
//            }
//          }
//          bundle.fwCtx.listeners
//            .serviceChanged(bundle.fwCtx.listeners.getMatchingServiceListeners(reference),
//                            new ServiceEvent(ServiceEvent.MODIFIED, reference),
//                            before);
//          bundle.fwCtx.listeners
//            .serviceChanged(before,
//                            new ServiceEvent(ServiceEvent.MODIFIED_ENDMATCH, reference),
//                            null);

  }

  void ctkServiceRegistration::unregister()
  {
    Q_D(ctkServiceRegistration);

    if (d->unregistering) return; // Silently ignore redundant unregistration.
    {
      QMutexLocker lock(&d->eventLock);
      if (d->unregistering) return;
      d->unregistering = true;

      if (d->available)
      {
        if (d->plugin)
        {
          d->plugin->fwCtx->services.removeServiceRegistration(this);
        }
      }
      else
      {
        throw std::logic_error("Service is unregistered");
      }
    }

    if (d->plugin)
    {
      //TODO
//      bundle.fwCtx.listeners
//            .serviceChanged(bundle.fwCtx.listeners.getMatchingServiceListeners(reference),
//                            new ServiceEvent(ServiceEvent.UNREGISTERING, reference),
//                            null);
    }

    {
      QMutexLocker lock(&d->eventLock);
      {
        QMutexLocker lock2(&d->propsLock);
        d->available = false;
        if (d->plugin)
        {
          for (QHashIterator<ctkPlugin*, QObject*> i(d->serviceInstances); i.hasNext();)
          {
            QObject* obj = i.next().value();
            try
            {
              // NYI, don't call inside lock
              qobject_cast<ctkServiceFactory*>(d->service)->ungetService(i.key(),
                                                         this,
                                                         obj);
            }
            catch (const std::exception& ue)
            {
              ctkPluginFrameworkEvent pfwEvent(ctkPluginFrameworkEvent::ERROR, d->plugin->q_func(), ue);
              d->plugin->fwCtx->listeners
                  .emitFrameworkEvent(pfwEvent);
            }
          }
        }
        d->plugin = 0;
        d->dependents.clear();
        d->service = 0;
        d->serviceInstances.clear();;
        d->unregistering = false;
      }
    }
  }

  bool ctkServiceRegistration::operator<(const ctkServiceRegistration& o) const
  {
    Q_D(const ctkServiceRegistration);
    return d->reference->operator <(*(o.d_func()->reference));

}
