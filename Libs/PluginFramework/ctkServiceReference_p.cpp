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

#include "ctkServiceReference_p.h"

#include <QObject>
#include <QMutexLocker>

#include "ctkPlugin_p.h"
#include "ctkPluginConstants.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkServiceFactory.h"
#include "ctkServiceException.h"
#include "ctkServices_p.h"
#include "ctkServiceRegistration_p.h"

//----------------------------------------------------------------------------
ctkServiceReferencePrivate::ctkServiceReferencePrivate(ctkServiceRegistrationPrivate* reg)
  : ref(1), registration(reg)
{
  if (registration) registration->ref.ref();
}

//----------------------------------------------------------------------------
ctkServiceReferencePrivate::~ctkServiceReferencePrivate()
{
  if (registration && !registration->ref.deref())
    delete registration;
}

//----------------------------------------------------------------------------
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
          catch (const ctkException& pe)
          {
            ctkServiceException se("ctkServiceFactory throw an exception",
                                   ctkServiceException::FACTORY_EXCEPTION, pe);
            plugin->d_func()->fwCtx->listeners.frameworkError(registration->plugin->q_func(), se);
            return 0;
          }
          if (s == 0)
          {
            ctkServiceException se("ctkServiceFactory produced null",
                                   ctkServiceException::FACTORY_ERROR);
            plugin->d_func()->fwCtx->listeners.frameworkError(registration->plugin->q_func(), se);
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
              plugin->d_func()->fwCtx->listeners.frameworkError(registration->plugin->q_func(), se);
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

//----------------------------------------------------------------------------
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
      catch (const ctkException& e)
      {
        plugin->d_func()->fwCtx->listeners.frameworkError(registration->plugin->q_func(), e);
      }
    }
    registration->dependents.remove(plugin);
  }

  return hadReferences;
}

//----------------------------------------------------------------------------
const ctkServiceProperties& ctkServiceReferencePrivate::getProperties() const
{
  return registration->properties;
}

//----------------------------------------------------------------------------
QVariant ctkServiceReferencePrivate::getProperty(const QString& key, bool lock) const
{
  if (lock)
  {
    QMutexLocker lock(&registration->propsLock);
    return registration->properties.value(key);
  }
  return registration->properties.value(key);
}
