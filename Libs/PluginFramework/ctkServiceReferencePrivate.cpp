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

#include "ctkServiceReferencePrivate.h"

#include <QObject>
#include <QMutexLocker>

#include "ctkPluginConstants.h"
#include "ctkServiceFactory.h"
#include "ctkServiceException.h"
#include "ctkPluginPrivate_p.h"
#include "ctkServiceRegistrationPrivate.h"
#include "ctkPluginFrameworkContext_p.h"

namespace ctk {

  ServiceReferencePrivate::ServiceReferencePrivate(ServiceRegistrationPrivate* reg)
    : registration(reg)
  {
  }

  QObject* ServiceReferencePrivate::getService(Plugin* plugin)
  {
    QObject* s = 0;
    {
      QMutexLocker lock(&registration->propsLock);
      if (registration->available)
      {
        int count = registration->dependents.value(plugin);
        if (count == 0)
        {
          QStringList classes =
              registration->properties.value(PluginConstants::OBJECTCLASS).toStringList();
          registration->dependents[plugin] = 1;
          if (ServiceFactory* serviceFactory = qobject_cast<ServiceFactory*>(registration->getService()))
          {
            try
            {
              s = serviceFactory->getService(plugin, registration->q_func());
            }
            catch (const std::exception& pe)
            {
              ServiceException se("ServiceFactory throw an exception",
                                  ServiceException::FACTORY_EXCEPTION, pe);
              plugin->d_func()->fwCtx->listeners.frameworkError
                (registration->plugin->q_func(), se);
              return 0;
            }
            if (s == 0) {
              ServiceException se("ServiceFactory produced null",
                                  ServiceException::FACTORY_ERROR);
              plugin->d_func()->fwCtx->listeners.frameworkError
                (registration->plugin->q_func(), se);
              return 0;
            }
            for (QStringListIterator i(classes); i.hasNext(); )
            {
              QString cls = i.next();
              if (!registration->plugin->fwCtx->services.checkServiceClass(s, cls))
              {
                ServiceException se(QString("ServiceFactory produced an object ") +
                                     "that did not implement: " + cls,
                                     ServiceException::FACTORY_ERROR);
                plugin->d_func()->fwCtx->listeners.frameworkError
                  (registration->plugin->q_func(), se);
                return 0;
              }
            }
            registration->serviceInstances.insert(plugin, s);
          }
          else
          {
            s = registration->getService();
          }
        }
        else
        {
          registration->dependents.insert(plugin, count + 1);
          if (qobject_cast<ServiceFactory*>(registration->getService()))
          {
            s = registration->serviceInstances.value(plugin);
          }
          else
          {
            s = registration->getService();
          }
        }
      }
    }
    return s;
  }

  /**
     * Unget the service object.
     *
     * @param bundle Bundle who wants remove service.
     * @param checkRefCounter If true decrement refence counter and remove service
     *                        if we reach zero. If false remove service without
     *                        checking refence counter.
     * @return True if service was remove or false if only refence counter was
     *         decremented.
     */
  bool ServiceReferencePrivate::ungetService(Plugin* plugin, bool checkRefCounter)
  {
    QMutexLocker lock(&registration->propsLock);
    bool hadReferences = false;
    if (registration->reference != 0)
    {
      bool removeService = false;

      int count= registration->dependents.value(plugin);
      if (count > 0)
      {
        hadReferences = true;
      }

      if(checkRefCounter)
      {
        if (count > 1)
        {
            registration->dependents[plugin] = count - 1;
        }
        else if(count == 1)
        {
          removeService = true;
        }
      }
      else
      {
        removeService = true;
      }

      if (removeService)
      {
        QObject* sfi = registration->serviceInstances[plugin];
        registration->serviceInstances.remove(plugin);
        if (sfi != 0)
        {
          try
          {
            qobject_cast<ServiceFactory*>(registration->getService())->ungetService(plugin,
                registration->q_func(), sfi);
          }
          catch (const std::exception& e)
          {
            plugin->d_func()->fwCtx->listeners.frameworkError(registration->plugin->q_func(), e);
          }
        }
        registration->dependents.remove(plugin);
      }
    }

    return hadReferences;
  }


}

