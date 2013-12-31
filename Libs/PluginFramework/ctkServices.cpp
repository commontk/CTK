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

#include "ctkServices_p.h"

#include <QStringListIterator>
#include <QMutexLocker>
#include <QBuffer>

#include <algorithm>

#include "ctkServiceFactory.h"
#include "ctkPluginConstants.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkServiceException.h"
#include "ctkServiceRegistration_p.h"
#include "ctkLDAPExpr_p.h"

//----------------------------------------------------------------------------
struct ServiceRegistrationComparator
{
  bool operator()(const ctkServiceRegistration& a, const ctkServiceRegistration& b) const
  {
    return a < b;
  }
};

//----------------------------------------------------------------------------
ctkDictionary ctkServices::createServiceProperties(const ctkDictionary& in,
                                                       const QStringList& classes,
                                                       long sid)
{
  static qlonglong nextServiceID = 1;
  ctkDictionary props = in;

  if (!classes.isEmpty())
  {
    props.insert(ctkPluginConstants::OBJECTCLASS, classes);
  }

  props.insert(ctkPluginConstants::SERVICE_ID, sid != -1 ? sid : nextServiceID++);

  return props;
}

//----------------------------------------------------------------------------
ctkServices::ctkServices(ctkPluginFrameworkContext* fwCtx)
  : mutex(), framework(fwCtx)
{

}

//----------------------------------------------------------------------------
ctkServices::~ctkServices()
{
  clear();
}

//----------------------------------------------------------------------------
void ctkServices::clear()
{
  services.clear();
  classServices.clear();
  framework = 0;
}

//----------------------------------------------------------------------------
ctkServiceRegistration ctkServices::registerService(ctkPluginPrivate* plugin,
                             const QStringList& classes,
                             QObject* service,
                             const ctkDictionary& properties)
{
  if (service == 0)
  {
    throw ctkInvalidArgumentException("Can't register 0 as a service");
  }

  // Check if service implements claimed classes and that they exist.
  for (QStringListIterator i(classes); i.hasNext();)
  {
    QString cls = i.next();
    if (cls.isEmpty())
    {
      throw ctkInvalidArgumentException("Can't register as null class");
    }

    if (!(qobject_cast<ctkServiceFactory*>(service)))
    {
      if (!checkServiceClass(service, cls))
      {
        QString msg = QString("Service class %1 is not an instance of %2. Maybe you forgot the Q_INTERFACES macro in the service class.")
            .arg(service->metaObject()->className()).arg(cls);
        throw ctkInvalidArgumentException(msg);
      }
    }
  }

  ctkServiceRegistration res(plugin, service,
                             createServiceProperties(properties, classes));
  {
    QMutexLocker lock(&mutex);
    services.insert(res, classes);
    for (QStringListIterator i(classes); i.hasNext(); )
    {
      QString currClass = i.next();
      QList<ctkServiceRegistration>& s = classServices[currClass];
      QList<ctkServiceRegistration>::iterator ip =
          std::lower_bound(s.begin(), s.end(), res, ServiceRegistrationComparator());
      s.insert(ip, res);
    }
  }

  ctkServiceReference r = res.getReference();
  plugin->fwCtx->listeners.serviceChanged(
      plugin->fwCtx->listeners.getMatchingServiceSlots(r),
      ctkServiceEvent(ctkServiceEvent::REGISTERED, r));
  return res;
}

//----------------------------------------------------------------------------
void ctkServices::updateServiceRegistrationOrder(const ctkServiceRegistration& sr,
                                              const QStringList& classes)
{
  QMutexLocker lock(&mutex);
  for (QStringListIterator i(classes); i.hasNext(); )
  {
    QList<ctkServiceRegistration>& s = classServices[i.next()];
    s.removeAll(sr);
    s.insert(std::lower_bound(s.begin(), s.end(), sr, ServiceRegistrationComparator()), sr);
  }
}

//----------------------------------------------------------------------------
bool ctkServices::checkServiceClass(QObject* service, const QString& cls) const
{
  return service->inherits(cls.toLatin1());
}

//----------------------------------------------------------------------------
QList<ctkServiceRegistration> ctkServices::get(const QString& clazz) const
{
  QMutexLocker lock(&mutex);
  return classServices.value(clazz);
}

//----------------------------------------------------------------------------
ctkServiceReference ctkServices::get(ctkPluginPrivate* plugin, const QString& clazz) const
{
  QMutexLocker lock(&mutex);
  try {
    QList<ctkServiceReference> srs = get_unlocked(clazz, QString(), plugin);
    if (framework->debug.service_reference)
    {
      qDebug() << "get service ref" << clazz << "for plugin"
               << plugin->location << " = " << srs.size() << "refs";
    }
    if (!srs.isEmpty()) {
      return srs.front();
    }
  }
  catch (const ctkInvalidArgumentException& )
  { }

  return ctkServiceReference();
}

//----------------------------------------------------------------------------
QList<ctkServiceReference> ctkServices::get(const QString& clazz, const QString& filter,
                                            ctkPluginPrivate* plugin) const
{
  QMutexLocker lock(&mutex);
  return get_unlocked(clazz, filter, plugin);
}

//----------------------------------------------------------------------------
QList<ctkServiceReference> ctkServices::get_unlocked(const QString& clazz, const QString& filter,
                                                     ctkPluginPrivate* plugin) const
{
  Q_UNUSED(plugin)

  QListIterator<ctkServiceRegistration>* s = 0;
  QList<ctkServiceRegistration> v;
  ctkLDAPExpr ldap;
  if (clazz.isEmpty())
  {
    if (!filter.isEmpty())
    {
      ldap = ctkLDAPExpr(filter);
      QSet<QString> matched;
      if (ldap.getMatchedObjectClasses(matched))
      {
        v.clear();
        foreach (QString className, matched)
        {
          const QList<ctkServiceRegistration>& cl = classServices[className];
          v += cl;
        }
        if (!v.isEmpty())
        {
          s = new QListIterator<ctkServiceRegistration>(v);
        }
        else
        {
          return QList<ctkServiceReference>();
        }
      }
      else
      {
        s = new QListIterator<ctkServiceRegistration>(services.keys());
      }
    }
    else
    {
      s = new QListIterator<ctkServiceRegistration>(services.keys());
    }
  }
  else
  {
    QList<ctkServiceRegistration> v = classServices.value(clazz);
    if (!v.isEmpty())
    {
      s = new QListIterator<ctkServiceRegistration>(v);
    }
    else
    {
      return QList<ctkServiceReference>();
    }
    if (!filter.isEmpty())
    {
      ldap = ctkLDAPExpr(filter);
    }
  }

  QList<ctkServiceReference> res;
  while (s->hasNext())
  {
    ctkServiceRegistration sr = s->next();
    ctkServiceReference sri = sr.getReference();

    if (filter.isEmpty() || ldap.evaluate(sr.d_func()->properties, false))
    {
      res.push_back(sri);
    }
  }

  delete s;

  return res;
}

//----------------------------------------------------------------------------
void ctkServices::removeServiceRegistration(const ctkServiceRegistration& sr)
{
  QMutexLocker lock(&mutex);

  QStringList classes = sr.d_func()->properties.value(ctkPluginConstants::OBJECTCLASS).toStringList();
  services.remove(sr);
  for (QStringListIterator i(classes); i.hasNext(); )
  {
    QString currClass = i.next();
    QList<ctkServiceRegistration>& s = classServices[currClass];
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

//----------------------------------------------------------------------------
QList<ctkServiceRegistration> ctkServices::getRegisteredByPlugin(ctkPluginPrivate* p) const
{
  QMutexLocker lock(&mutex);

  QList<ctkServiceRegistration> res;
  for (QHashIterator<ctkServiceRegistration, QStringList> i(services); i.hasNext(); )
  {
    ctkServiceRegistration sr = i.next().key();
    if (sr.d_func()->plugin == p)
    {
      res.push_back(sr);
    }
  }
  return res;
}

//----------------------------------------------------------------------------
QList<ctkServiceRegistration> ctkServices::getUsedByPlugin(QSharedPointer<ctkPlugin> p) const
{
  QMutexLocker lock(&mutex);

  QList<ctkServiceRegistration> res;
  for (QHashIterator<ctkServiceRegistration, QStringList> i(services); i.hasNext(); )
  {
    ctkServiceRegistration sr = i.next().key();
    if (sr.d_func()->isUsedByPlugin(p))
    {
      res.push_back(sr);
    }
  }
  return res;
}

