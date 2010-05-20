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

#include "ctkServices_p.h"

#include <QStringListIterator>
#include <QMutexLocker>
#include <QBuffer>

#include <algorithm>

#include "ctkServiceFactory.h"
#include "ctkPluginConstants.h"
#include "ctkServiceRegistrationPrivate.h"
#include "ctkQtServiceRegistration_p.h"

namespace ctk {

  using namespace QtMobility;

  //TODO: this is just a mock class. Wait for the real thing
  class LDAPExpr
  {

  public:

    LDAPExpr(const QString& filter)
    {

    }

    /**
     * Get object class set matched by this LDAP expression. This will not work
     * with wildcards and NOT expressions. If a set can not be determined return null.
     *
     * @return Set of classes matched, otherwise <code>null</code>.
     */
    QSet<QString> getMatchedObjectClasses() const
    {
      QSet<QString> objClasses;
      return objClasses;
    }


    /**
     * Evaluate this LDAP filter.
     */
    bool evaluate(const ServiceProperties& p, bool matchCase)
    {
      return true;
    }

  };


  struct ServiceRegistrationComparator
  {
    bool operator()(const ServiceRegistration* a, const ServiceRegistration* b) const
    {
      return *a < *b;
    }
  };

  ServiceProperties Services::createServiceProperties(const ServiceProperties& in,
                                                      const QStringList& classes,
                                                      long sid)
  {
    static qlonglong nextServiceID = 1;
    ServiceProperties props;

    if (!in.isEmpty())
    {
      for (ServiceProperties::const_iterator it = in.begin(); it != in.end(); ++it)
      {
        const QString key = it.key();
        const QString lcKey = it.key().toLower();
        for (QListIterator<QString> i(props.keys()); i.hasNext(); )
        {
          if (lcKey == i.next())
          {
            throw std::invalid_argument(std::string("Several entries for property: ") + key.toStdString());
          }
        }

        props.insert(lcKey, in.value(key));
      }
    }

    if (!classes.isEmpty())
    {
      props.insert(PluginConstants::OBJECTCLASS, classes);
    }

    props.insert(PluginConstants::SERVICE_ID, sid != -1 ? sid : nextServiceID++);

    return props;
  }


Services::Services(PluginFrameworkContext* fwCtx)
  : mutex(QMutex::Recursive), framework(fwCtx)
{

}

Services::~Services()
{
  clear();
}

void Services::clear()
{
  QList<ServiceRegistration*> serviceRegs = services.keys();
  qDeleteAll(serviceRegs);
  services.clear();
  classServices.clear();
  framework = 0;
}

ServiceRegistration* Services::registerService(PluginPrivate* plugin,
                             const QStringList& classes,
                             QObject* service,
                             const ServiceProperties& properties)
{
  if (service == 0)
  {
    throw std::invalid_argument("Can't register 0 as a service");
  }

  // Check if service implements claimed classes and that they exist.
  for (QStringListIterator i(classes); i.hasNext();)
  {
    QString cls = i.next();
    if (cls.isEmpty())
    {
      throw std::invalid_argument("Can't register as null class");
    }

    if (!(qobject_cast<ServiceFactory*>(service)))
    {
      if (!checkServiceClass(service, cls))
      {
        throw std::invalid_argument
            (std::string("Service object is not an instance of ") + cls.toStdString());
      }
    }
  }

  ServiceRegistration* res = new ServiceRegistration(plugin, service,
                                createServiceProperties(properties, classes));
  {
    QMutexLocker lock(&mutex);
    services.insert(res, classes);
    for (QStringListIterator i(classes); i.hasNext(); )
    {
      QString currClass = i.next();
      QList<ServiceRegistration*>& s = classServices[currClass];
      QList<ServiceRegistration*>::iterator ip =
          std::lower_bound(s.begin(), s.end(), res, ServiceRegistrationComparator());
      s.insert(ip, res);
    }
  }

  ServiceReference* r = res->getReference();
  // TODO
  //Listeners l = bundle.fwCtx.listeners;
  //l.serviceChanged(l.getMatchingServiceListeners(r),
  //                 new ServiceEvent(ServiceEvent.REGISTERED, r),
  //                 null);
  return res;
}

void Services::registerService(PluginPrivate* plugin, QByteArray serviceDescription)
{
  QMutexLocker lock(&mutex);

  QBuffer serviceBuffer(&serviceDescription);
  qServiceManager.addService(&serviceBuffer);
  QServiceManager::Error error = qServiceManager.error();
  if (!(error == QServiceManager::NoError || error == QServiceManager::ServiceAlreadyExists))
  {
    throw std::invalid_argument(std::string("Registering the service descriptor for plugin ")
                                + plugin->symbolicName.toStdString() + " failed: " +
                                getQServiceManagerErrorString(error).toStdString());
  }

  QString serviceName = plugin->symbolicName + "_" + plugin->version.toString();
  QList<QServiceInterfaceDescriptor> descriptors = qServiceManager.findInterfaces(serviceName);

  if (descriptors.isEmpty())
  {
    qDebug().nospace() << "Warning: No interfaces found for service name " << serviceName
        << " in plugin " << plugin->symbolicName << " (Version " << plugin->version.toString() << ")";
  }

  QListIterator<QServiceInterfaceDescriptor> it(descriptors);
  while (it.hasNext())
  {
    QServiceInterfaceDescriptor descr = it.next();
    qDebug() << "Registering:" << descr.interfaceName();
    QStringList classes;
    ServiceProperties props;

    QStringList customKeys = descr.customAttributes();
    QStringListIterator keyIt(customKeys);
    bool classAttrFound = false;
    while (keyIt.hasNext())
    {
      QString key = keyIt.next();
      if (key == PluginConstants::OBJECTCLASS)
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
      throw std::invalid_argument(std::string("The custom attribute \"") +
                                  PluginConstants::OBJECTCLASS.toStdString() +
                                  "\" is missing in the interface description of \"" +
                                  descr.interfaceName().toStdString());
    }

    ServiceRegistration* res = new QtServiceRegistration(plugin,
                                                         descr,
                                                         createServiceProperties(props, classes));
    services.insert(res, classes);
    for (QStringListIterator i(classes); i.hasNext(); )
    {
      QString currClass = i.next();
      QList<ServiceRegistration*>& s = classServices[currClass];
      QList<ServiceRegistration*>::iterator ip =
          std::lower_bound(s.begin(), s.end(), res, ServiceRegistrationComparator());
      s.insert(ip, res);
    }

    //ServiceReference* r = res->getReference();
    // TODO
    //Listeners l = bundle.fwCtx.listeners;
    //l.serviceChanged(l.getMatchingServiceListeners(r),
    //                 new ServiceEvent(ServiceEvent.REGISTERED, r),
    //                 null);
  }
}

QString Services::getQServiceManagerErrorString(QServiceManager::Error error)
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

void Services::updateServiceRegistrationOrder(ServiceRegistration* sr,
                                              const QStringList& classes)
{
  QMutexLocker lock(&mutex);
  for (QStringListIterator i(classes); i.hasNext(); )
  {
    QList<ServiceRegistration*>& s = classServices[i.next()];
    s.removeAll(sr);
    s.insert(std::lower_bound(s.begin(), s.end(), sr, ServiceRegistrationComparator()), sr);
  }
}

bool Services::checkServiceClass(QObject* service, const QString& cls) const
{
  return service->inherits(cls.toAscii());
}


QList<ServiceRegistration*> Services::get(const QString& clazz) const
{
  QMutexLocker lock(&mutex);
  return classServices.value(clazz);
}


ServiceReference* Services::get(PluginPrivate* plugin, const QString& clazz) const
{
  QMutexLocker lock(&mutex);
  try {
    QList<ServiceReference*> srs = get(clazz, QString());
    qDebug() << "get service ref" << clazz << "for plugin"
             << plugin->location << " = " << srs;

    if (!srs.isEmpty()) {
      return srs.front();
    }
  }
  catch (const std::invalid_argument& )
  { }

  return 0;
}


QList<ServiceReference*> Services::get(const QString& clazz, const QString& filter) const
{
  QMutexLocker lock(&mutex);

  QListIterator<ServiceRegistration*>* s = 0;
  LDAPExpr ldap("");
  if (clazz.isEmpty())
  {
    if (!filter.isEmpty())
    {
      ldap = LDAPExpr(filter);
      QSet<QString> matched = ldap.getMatchedObjectClasses();
      if (!matched.isEmpty())
      {
        //TODO
//        ArrayList v = null;
//        boolean vReadOnly = true;;
//        for (Iterator i = matched.iterator(); i.hasNext(); ) {
//          ArrayList cl = (ArrayList) classServices.get(i.next());
//          if (cl != null) {
//            if (v == null) {
//              v = cl;
//            } else {
//              if (vReadOnly) {
//                v = new ArrayList(v);
//                vReadOnly = false;
//              }
//              v.addAll(cl);
//            }
//          }
//        }
//        if (v != null) {
//          s = v.iterator();
//        } else {
//          return null;
//        }
      }
      else
      {
        s = new QListIterator<ServiceRegistration*>(services.keys());
      }
    }
    else
    {
      s = new QListIterator<ServiceRegistration*>(services.keys());
    }
  }
  else
  {
    QList<ServiceRegistration*> v = classServices.value(clazz);
    if (!v.isEmpty())
    {
      s = new QListIterator<ServiceRegistration*>(v);
    }
    else
    {
      return QList<ServiceReference*>();
    }
    if (!filter.isEmpty())
    {
      ldap = LDAPExpr(filter);
    }
  }

  QList<ServiceReference*> res;
  while (s->hasNext())
  {
    ServiceRegistration* sr = s->next();
    ServiceReference* sri = sr->getReference();

    if (filter.isEmpty() || ldap.evaluate(sr->d_func()->properties, false))
    {
      res.push_back(sri);
    }
  }

  delete s;

  return res;
}


void Services::removeServiceRegistration(ServiceRegistration* sr)
{
  QMutexLocker lock(&mutex);

  QStringList classes = sr->d_func()->properties.value(PluginConstants::OBJECTCLASS).toStringList();
  services.remove(sr);
  for (QStringListIterator i(classes); i.hasNext(); )
  {
    QString currClass = i.next();
    QList<ServiceRegistration*>& s = classServices[currClass];
    if (s.size() > 1)
    {
      s.removeAll(sr);
    }
    else
    {
      classServices.remove(currClass);
    }
  }
}


QList<ServiceRegistration*> Services::getRegisteredByPlugin(PluginPrivate* p) const
{
  QMutexLocker lock(&mutex);

  QList<ServiceRegistration*> res;
  for (QHashIterator<ServiceRegistration*, QStringList> i(services); i.hasNext(); )
  {
    ServiceRegistration* sr = i.next().key();
    if (sr->d_func()->plugin = p)
    {
      res.push_back(sr);
    }
  }
  return res;
}


QList<ServiceRegistration*> Services::getUsedByPlugin(Plugin* p) const
{
  QMutexLocker lock(&mutex);

  QList<ServiceRegistration*> res;
  for (QHashIterator<ServiceRegistration*, QStringList> i(services); i.hasNext(); )
  {
    ServiceRegistration* sr = i.next().key();
    if (sr->d_func()->isUsedByPlugin(p))
    {
      res.push_back(sr);
    }
  }
  return res;
}

}

