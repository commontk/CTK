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

#include "ctkServiceReferencePrivate.h"

#include <QObject>
#include <QMutexLocker>

#include "ctkPluginConstants.h"
#include "ctkServiceFactory.h"
#include "ctkServiceException.h"
#include "ctkPluginPrivate_p.h"

#include "ctkServices_p.h"
#include "ctkServiceRegistrationPrivate.h"
#include "ctkPluginFrameworkContext_p.h"


ctkServiceReferencePrivate::ctkServiceReferencePrivate(ctkServiceRegistrationPrivate* reg)
  : ref(1), registration(reg)
{
}

QObject* ctkServiceReferencePrivate::getService(QSharedPointer<ctkPlugin> plugin)
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
            registration->properties.value(ctkPluginConstants::OBJECTCLASS).toStringList();
        registration->dependents[plugin] = 1;
        if (ctkServiceFactory* serviceFactory = qobject_cast<ctkServiceFactory*>(registration->getService()))
        {
          try
          {
            s = serviceFactory->getService(plugin, ctkServiceRegistration(registration));
          }
          catch (const std::exception& pe)
          {
            ctkServiceException se("ctkServiceFactory throw an exception",
                                   ctkServiceException::FACTORY_EXCEPTION, &pe);
            plugin->d_func()->fwCtx->listeners.frameworkError
                (registration->plugin->q_func().data(), se);
            return 0;
          }
          if (s == 0) {
            ctkServiceException se("ctkServiceFactory produced null",
                                   ctkServiceException::FACTORY_ERROR);
            plugin->d_func()->fwCtx->listeners.frameworkError
                (registration->plugin->q_func().data(), se);
            return 0;
          }
          for (QStringListIterator i(classes); i.hasNext(); )
          {
            QString cls = i.next();
            if (!registration->plugin->fwCtx->services->checkServiceClass(s, cls))
            {
              ctkServiceException se(QString("ctkServiceFactory produced an object ") +
                                     "that did not implement: " + cls,
                                     ctkServiceException::FACTORY_ERROR);
              plugin->d_func()->fwCtx->listeners.frameworkError
                  (registration->plugin->q_func().data(), se);
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
        if (qobject_cast<ctkServiceFactory*>(registration->getService()))
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

bool ctkServiceReferencePrivate::ungetService(QSharedPointer<ctkPlugin> plugin, bool checkRefCounter)
{
  QMutexLocker lock(&registration->propsLock);
  bool hadReferences = false;
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
        qobject_cast<ctkServiceFactory*>(
              registration->getService())->ungetService(plugin, ctkServiceRegistration(registration), sfi);
      }
      catch (const std::exception& e)
      {
        plugin->d_func()->fwCtx->listeners.frameworkError(registration->plugin->q_func().data(), e);
      }
    }
    registration->dependents.remove(plugin);
  }

  return hadReferences;
}

ServiceProperties ctkServiceReferencePrivate::getProperties() const
{
  return registration->properties;
}
