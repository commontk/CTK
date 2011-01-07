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


#include "ctkQtMobilityServiceRuntime_p.h"

#include "ctkQtMobilityServiceConstants_p.h"
#include "ctkQtMobilityServiceActivator_p.h"
#include "ctkQtMobilityServiceFactory_p.h"

#include <ctkPluginContext.h>
#include <ctkPlugin.h>
#include <service/log/ctkLogService.h>
#include <ctkPluginConstants.h>
#include <ctkServiceRegistration.h>

#include <QBuffer>

ctkQtMobilityServiceRuntime::ctkQtMobilityServiceRuntime(ctkPluginContext* pc)
  : pc(pc)
{
}

void ctkQtMobilityServiceRuntime::start()
{
  pc->connectPluginListener(this, SLOT(pluginChanged(ctkPluginEvent)), Qt::DirectConnection);
  QList<QSharedPointer<ctkPlugin> > plugins = pc->getPlugins();
  foreach(QSharedPointer<ctkPlugin> plugin, plugins)
  {
    if ((plugin->getState() & (ctkPlugin::ACTIVE | ctkPlugin::STARTING)) != 0)
    {
      processPlugin(plugin);
    }
  }
}

void ctkQtMobilityServiceRuntime::stop()
{
  this->disconnect(this, SLOT(pluginChanged(ctkPluginEvent)));
//  Bundle [] b = (Bundle [])bundleComponents.keySet().toArray(new Bundle[bundleComponents.size()]);
//      for (int i = 0; i < b.length; i++) {
//        removeBundle(b[i], ComponentConstants.DEACTIVATION_REASON_DISABLED);
//      }
}

QServiceManager& ctkQtMobilityServiceRuntime::getQServiceManager()
{
  return qServiceManager;
}

void ctkQtMobilityServiceRuntime::pluginChanged(const ctkPluginEvent& pe)
{
  QSharedPointer<ctkPlugin> plugin = pe.getPlugin();

  ctkPluginEvent::Type eventType = pe.getType();
  if (eventType == ctkPluginEvent::LAZY_ACTIVATION)
  {
    lazy.insert(plugin);
    processPlugin(plugin);
  }
  else if (eventType == ctkPluginEvent::STARTED)
  {
    if (!lazy.remove(plugin))
    {
      processPlugin(plugin);
    }
  }
  else if (eventType == ctkPluginEvent::STOPPING)
  {
    lazy.remove(plugin);
    removePlugin(plugin);
  }
}

void ctkQtMobilityServiceRuntime::processPlugin(QSharedPointer<ctkPlugin> plugin)
{
  QHash<QString, QString> headers = plugin->getHeaders();
  QHash<QString, QString>::const_iterator it = headers.find(ctkQtMobilityServiceConstants::SERVICE_DESCRIPTOR);
  ctkLogService* log = ctkQtMobilityServiceActivator::getLogService();
  CTK_DEBUG(log)
      << "Process header " << ctkQtMobilityServiceConstants::SERVICE_DESCRIPTOR
      << " for plugin #" << plugin->getPluginId() << ": " << (it != headers.end() ? it.value() : "[missing]");

  if (it != headers.end())
  {
    QString sd = it.value();
    if (sd.isEmpty())
    {
      QString msg = QString("Header ") + ctkQtMobilityServiceConstants::SERVICE_DESCRIPTOR + " empty.";
      ctkQtMobilityServiceActivator::logError(plugin->getPluginContext(), msg);
      return;
    }

    QByteArray serviceDescription = plugin->getResource(sd);
    QBuffer serviceBuffer(&serviceDescription);
    qServiceManager.addService(&serviceBuffer);
    QServiceManager::Error error = qServiceManager.error();
    if (!(error == QServiceManager::NoError || error == QServiceManager::ServiceAlreadyExists))
    {
      QString msg = QString("Registering the QtMobility service descriptor failed: ") +
          getQServiceManagerErrorString(error);
      ctkQtMobilityServiceActivator::logError(plugin->getPluginContext(), msg);
      return;
    }

    QString serviceName = plugin->getSymbolicName() + "_" + plugin->getVersion().toString();
    QList<QServiceInterfaceDescriptor> descriptors = qServiceManager.findInterfaces(serviceName);

    if (descriptors.isEmpty())
    {
      QString msg = QString("No interfaces found for service name ") + serviceName;
      ctkQtMobilityServiceActivator::logWarning(plugin->getPluginContext(), msg);
      return;
    }

    QListIterator<QServiceInterfaceDescriptor> it(descriptors);
    while (it.hasNext())
    {
      QServiceInterfaceDescriptor descr = it.next();
      CTK_DEBUG(ctkQtMobilityServiceActivator::getLogService()) << "Registering:" << descr.interfaceName();
      QStringList classes;
      ServiceProperties props;

      QStringList customKeys = descr.customAttributes();
      QStringListIterator keyIt(customKeys);
      bool classAttrFound = false;
      while (keyIt.hasNext())
      {
        QString key = keyIt.next();
        if (key == ctkPluginConstants::OBJECTCLASS)
        {
          classAttrFound = true;
          classes << descr.customAttribute(key);
        }
        else
        {
          props.insert(key, descr.customAttribute(key));
        }
      }

      if (!classAttrFound)
      {
        QString msg = QString("The custom attribute \"") + ctkPluginConstants::OBJECTCLASS
            + "\" is missing in the interface description of \"" + descr.interfaceName();
        ctkQtMobilityServiceActivator::logError(plugin->getPluginContext(), msg);
        continue;
      }

      ctkQtMobilityServiceFactory* serviceObject = new ctkQtMobilityServiceFactory(descr, this, plugin);
      ctkServiceRegistration serviceReg = plugin->getPluginContext()->registerService(classes, serviceObject, props);

      if (serviceReg)
      {
        mapPluginToServiceFactory.insert(plugin, serviceObject);
        mapPluginToServiceRegistration.insert(plugin, serviceReg);
      }
      else
      {
        QString msg = QString("Could not register QtMobility service ") + descr.serviceName() + " "
            + descr.interfaceName();
        ctkQtMobilityServiceActivator::logError(plugin->getPluginContext(), msg);
        continue;
      }
    }
  }
}

void ctkQtMobilityServiceRuntime::removePlugin(QSharedPointer<ctkPlugin> plugin)
{
  CTK_DEBUG(ctkQtMobilityServiceActivator::getLogService())
      << "Remove " << plugin->getSymbolicName() << " from QtMobSR";

  QList<ctkQtMobilityServiceFactory*> serviceFactories = mapPluginToServiceFactory.values(plugin);
  QList<ctkServiceRegistration> serviceRegs = mapPluginToServiceRegistration.values(plugin);
  foreach(ctkServiceRegistration serviceReg, serviceRegs)
  {
    serviceReg.unregister();
  }

  mapPluginToServiceRegistration.remove(plugin);
  mapPluginToServiceFactory.remove(plugin);

  qDeleteAll(serviceFactories);
}

QString ctkQtMobilityServiceRuntime::getQServiceManagerErrorString(QServiceManager::Error error)
{
  switch (error)
  {
  case QServiceManager::NoError:
    return QString("No error occurred.");
  case QServiceManager::StorageAccessError:
    return QString("The service data storage is not accessible. This could be because the caller does not have the required permissions.");
  case QServiceManager::InvalidServiceLocation:
    return QString("The service was not found at its specified location.");
  case QServiceManager::InvalidServiceXml:
    return QString("The XML defining the service metadata is invalid.");
  case QServiceManager::InvalidServiceInterfaceDescriptor:
    return QString("The service interface descriptor is invalid, or refers to an interface implementation that cannot be accessed in the current scope.");
  case QServiceManager::ServiceAlreadyExists:
    return QString("Another service has previously been registered with the same location.");
  case QServiceManager::ImplementationAlreadyExists:
    return QString("Another service that implements the same interface version has previously been registered.");
  case QServiceManager::PluginLoadingFailed:
    return QString("The service plugin cannot be loaded.");
  case QServiceManager::ComponentNotFound:
    return QString("The service or interface implementation has not been registered.");
  case QServiceManager::ServiceCapabilityDenied:
    return QString("The security session does not allow the service based on its capabilities.");
  case QServiceManager::UnknownError:
    return QString("An unknown error occurred.");
  default:
    return QString("Unknown error enum.");
  }
}
