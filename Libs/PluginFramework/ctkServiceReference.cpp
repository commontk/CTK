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

#include "ctkServiceReference.h"
#include "ctkServiceReferencePrivate.h"
#include "ctkServiceRegistrationPrivate.h"
#include "ctkPluginPrivate_p.h"
#include "ctkPluginConstants.h"

#include <QStringList>
#include <QMutexLocker>
#include <QDebug>

ctkServiceReference::ctkServiceReference()
  : d_ptr(new ctkServiceReferencePrivate(0))
{

}

ctkServiceReference::ctkServiceReference(const ctkServiceReference& ref)
  : d_ptr(ref.d_ptr)
{
  d_func()->ref.ref();
}

ctkServiceReference::ctkServiceReference(ctkServiceRegistrationPrivate* reg)
  : d_ptr(new ctkServiceReferencePrivate(reg))
{

}

ctkServiceReference::operator bool() const
{
  return getPlugin();
}

ctkServiceReference::~ctkServiceReference()
{
  if (!d_func()->ref.deref())
    delete d_ptr;
}

QVariant ctkServiceReference::getProperty(const QString& key) const
{
  Q_D(const ctkServiceReference);

  QMutexLocker lock(&d->registration->propsLock);

  return d->registration->properties.value(key);
}

QStringList ctkServiceReference::getPropertyKeys() const
{
  Q_D(const ctkServiceReference);

  QMutexLocker lock(&d->registration->propsLock);

  QStringList result;
  foreach (ctkCaseInsensitiveString key, d->registration->properties.keys())
  {
    result << key;
  }
  return result;
}

QSharedPointer<ctkPlugin> ctkServiceReference::getPlugin() const
{
  if (d_func()->registration == 0 || d_func()->registration->plugin == 0)
  {
    return QSharedPointer<ctkPlugin>();
  }

  return d_func()->registration->plugin->q_func().toStrongRef();
}

QList<QSharedPointer<ctkPlugin> > ctkServiceReference::getUsingPlugins() const
{
  Q_D(const ctkServiceReference);

  QMutexLocker lock(&d->registration->propsLock);

  return d->registration->dependents.keys();
}

bool ctkServiceReference::operator<(const ctkServiceReference& reference) const
{
  bool sameFw = d_func()->registration->plugin->fwCtx == reference.d_func()->registration->plugin->fwCtx;
  if (!sameFw)
  {
    throw std::invalid_argument("Can not compare service references "
                                "belonging to different framework "
                                "instances.");
  }

  int r1 = getProperty(ctkPluginConstants::SERVICE_RANKING).toInt();
  int r2 = reference.getProperty(ctkPluginConstants::SERVICE_RANKING).toInt();

  if (r1 != r2)
  {
    // use ranking if ranking differs
    return r1 < r2 ? false : true;
  }
  else
  {
    qlonglong id1 = getProperty(ctkPluginConstants::SERVICE_ID).toLongLong();
    qlonglong id2 = reference.getProperty(ctkPluginConstants::SERVICE_ID).toLongLong();

    // otherwise compare using IDs,
    // is less than if it has a higher ID.
    return id2< id1;
  }
}

bool ctkServiceReference::operator==(const ctkServiceReference& reference) const
{
  return d_func()->registration == reference.d_func()->registration;
}

ctkServiceReference& ctkServiceReference::operator=(const ctkServiceReference& reference)
{
  ctkServiceReferencePrivate* curr_d = d_func();
  d_ptr = reference.d_ptr;
  d_ptr->ref.ref();

  if (!curr_d->ref.deref())
    delete curr_d;

  return *this;
}

uint qHash(const ctkServiceReference& serviceRef)
{
  return qHash(serviceRef.d_func()->registration);
}

QDebug operator<<(QDebug dbg, const ctkServiceReference& serviceRef)
{
  dbg.nospace() << "Reference for service object registered from "
      << serviceRef.getPlugin()->getSymbolicName() << " " << serviceRef.getPlugin()->getVersion()
      << " (";
  int i = serviceRef.getPropertyKeys().size();
  foreach(QString key, serviceRef.getPropertyKeys())
  {
    dbg.nospace() << key << "=" << serviceRef.getProperty(key).toString();
    if (--i > 0) dbg.nospace() << ",";
  }
  dbg.nospace() << ")";

  return dbg.maybeSpace();
}
