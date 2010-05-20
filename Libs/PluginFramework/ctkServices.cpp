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


  using namespace QtMobility;

  //TODO: this is just a mock class. ctkWait for the real thing
  class ctkLDAPExpr
  {

  public:

    ctkLDAPExpr(const QString& filter)
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
    bool operator()(const ctkServiceRegistration* a, const ctkServiceRegistration* b) const
    {
      return *a < *b;
    }
  };

  ServiceProperties ctkServices::createServiceProperties(const ServiceProperties& in,
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


ctkServices::ctkServices(ctkPluginFrameworkContext* fwCtx)
  : mutex(QMutex::Recursive), framework(fwCtx)
{

}

ctkServices::~ctkServices()
{
  clear();
}

void ctkServices::clear()
{
  QList<ctkServiceRegistration*> serviceRegs = services.keys();
  qDeleteAll(serviceRegs);
  services.clear();
  classServices.clear();
  framework = 0;
}

ctkServiceRegistration* ctkServices::registerService(ctkPluginPrivate* plugin,
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

    if (!(qobject_cast<ctkServiceFactory*>(service)))
    {
      if (!checkServiceClass(service, cls))
      {
        throw std::invalid_argument
            (std::string("Service object is not an instance of ") + cls.toStdString());
      }
    }
  }

  ctkServiceRegistration* res = new ctkServiceRegistration(plugin, service,
                                createServiceProperties(properties, classes));
  {
    QMutexLocker lock(&mutex);
    services.insert(res, classes);
    for (QStringListIterator i(classes); i.hasNext(); )
    {
      QString currClass = i.next();
      QList<ctkServiceRegistration*>& s = classServices[currClass];
      QList<ctkServiceRegistration*>::iterator ip =
          std::lower_bound(s.begin(), s.end(), res, ServiceRegistrationComparator());
      s.insert(ip, res);
    }
  }

  ctkServiceReference* r = res->getReference();
  // TODO
  //Listeners l = bundle.fwCtx.listeners;
  //l.serviceChanged(l.getMatchingServiceListeners(r),
  //                 new ServiceEvent(ServiceEvent.REGISTERED, r),
  //                 null);
  return res;
}

void ctkServices::registerService(ctkPluginPrivate* plugin, QByteArray serviceDescription)
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
        << " in plugin " << plugin->symbolicName << " (ctkVersion " << plugin->version.toString() << ")";
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

    ctkServiceRegistration* res = new ctkQtServiceRegistration(plugin,
                                                         descr,
                                                         createServiceProperties(props, classes));
    services.insert(res, classes);
    for (QStringListIterator i(classes); i.hasNext(); )
    {
      QString currClass = i.next();
      QList<ctkServiceRegistration*>& s = classServices[currClass];
      QList<ctkServiceRegistration*>::iterator ip =
          std::lower_bound(s.begin(), s.end(), res, ServiceRegistrationComparator());
      s.insert(ip, res);
    }

    //ctkServiceReference* r = res->getReference();
    // TODO
    //Listeners l = bundle.fwCtx.listeners;
    //l.serviceChanged(l.getMatchingServiceListeners(r),
    //                 new ServiceEvent(ServiceEvent.REGISTERED, r),
    //                 null);
  }
}

QString ctkServices::getQServiceManagerErrorString(QServiceManager::Error error)
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

void ctkServices::updateServiceRegistrationOrder(ctkServiceRegistration* sr,
                                              const QStringList& classes)
{
  QMutexLocker lock(&mutex);
  for (QStringListIterator i(classes); i.hasNext(); )
  {
    QList<ctkServiceRegistration*>& s = classServices[i.next()];
    s.removeAll(sr);
    s.insert(std::lower_bound(s.begin(), s.end(), sr, ServiceRegistrationComparator()), sr);
  }
}

bool ctkServices::checkServiceClass(QObject* service, const QString& cls) const
{
  return service->inherits(cls.toAscii());
}


QList<ctkServiceRegistration*> ctkServices::get(const QString& clazz) const
{
  QMutexLocker lock(&mutex);
  return classServices.value(clazz);
}


ctkServiceReference* ctkServices::get(ctkPluginPrivate* plugin, const QString& clazz) const
{
  QMutexLocker lock(&mutex);
  try {
    QList<ctkServiceReference*> srs = get(clazz, QString());
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


QList<ctkServiceReference*> ctkServices::get(const QString& clazz, const QString& filter) const
{
  QMutexLocker lock(&mutex);

  QListIterator<ctkServiceRegistration*>* s = 0;
  ctkLDAPExpr ldap("");
  if (clazz.isEmpty())
  {
    if (!filter.isEmpty())
    {
      ldap = ctkLDAPExpr(filter);
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
        s = new QListIterator<ctkServiceRegistration*>(services.keys());
      }
    }
    else
    {
      s = new QListIterator<ctkServiceRegistration*>(services.keys());
    }
  }
  else
  {
    QList<ctkServiceRegistration*> v = classServices.value(clazz);
    if (!v.isEmpty())
    {
      s = new QListIterator<ctkServiceRegistration*>(v);
    }
    else
    {
      return QList<ctkServiceReference*>();
    }
    if (!filter.isEmpty())
    {
      ldap = ctkLDAPExpr(filter);
    }
  }

  QList<ctkServiceReference*> res;
  while (s->hasNext())
  {
    ctkServiceRegistration* sr = s->next();
    ctkServiceReference* sri = sr->getReference();

    if (filter.isEmpty() || ldap.evaluate(sr->d_func()->properties, false))
    {
      res.push_back(sri);
    }
  }

  delete s;

  return res;
}


void ctkServices::removeServiceRegistration(ctkServiceRegistration* sr)
{
  QMutexLocker lock(&mutex);

  QStringList classes = sr->d_func()->properties.value(PluginConstants::OBJECTCLASS).toStringList();
  services.remove(sr);
  for (QStringListIterator i(classes); i.hasNext(); )
  {
    QString currClass = i.next();
    QList<ctkServiceRegistration*>& s = classServices[currClass];
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


QList<ctkServiceRegistration*> ctkServices::getRegisteredByPlugin(ctkPluginPrivate* p) const
{
  QMutexLocker lock(&mutex);

  QList<ctkServiceRegistration*> res;
  for (QHashIterator<ctkServiceRegistration*, QStringList> i(services); i.hasNext(); )
  {
    ctkServiceRegistration* sr = i.next().key();
    if (sr->d_func()->plugin = p)
    {
      res.push_back(sr);
    }
  }
  return res;
}


QList<ctkServiceRegistration*> ctkServices::getUsedByPlugin(ctkPlugin* p) const
{
  QMutexLocker lock(&mutex);

  QList<ctkServiceRegistration*> res;
  for (QHashIterator<ctkServiceRegistration*, QStringList> i(services); i.hasNext(); )
  {
    ctkServiceRegistration* sr = i.next().key();
    if (sr->d_func()->isUsedByPlugin(p))
    {
      res.push_back(sr);
    }
  }
  return res;
}


