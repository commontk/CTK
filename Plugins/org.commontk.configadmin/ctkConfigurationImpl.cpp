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


#include "ctkConfigurationImpl_p.h"

#include "ctkConfigurationAdminFactory_p.h"
#include "ctkConfigurationStore_p.h"

#include <service/cm/ctkConfigurationAdmin.h>
#include <ctkPluginConstants.h>

#include <QThread>

ctkConfigurationImpl::ctkConfigurationImpl(ctkConfigurationAdminFactory* configurationAdminFactory,
                                           ctkConfigurationStore* configurationStore,
                                           const QString& factoryPid, const QString& pid,
                                           const QString& pluginLocation)
  : configurationAdminFactory(configurationAdminFactory),
    configurationStore(configurationStore), pluginLocation(pluginLocation),
    factoryPid(factoryPid), pid(pid), deleted(false), lockedCount(0), lockHolder(0)
{

}

ctkConfigurationImpl::ctkConfigurationImpl(ctkConfigurationAdminFactory* configurationAdminFactory,
                                           ctkConfigurationStore* configurationStore,
                                           const ctkDictionary& dictionary)
  : configurationAdminFactory(configurationAdminFactory),
    configurationStore(configurationStore), deleted(false), lockedCount(0),
    lockHolder(0)
{
  pid = dictionary.value(ctkPluginConstants::SERVICE_PID).toString();
  factoryPid = dictionary.value(ctkConfigurationAdmin::SERVICE_FACTORYPID).toString();
  pluginLocation = dictionary.value(ctkConfigurationAdmin::SERVICE_PLUGINLOCATION).toString();
  updateDictionary(dictionary);
}

void ctkConfigurationImpl::remove()
{
  {
    ctkConfigurationImplLocker l(this);
    checkDeleted();
    deleted = true;
    configurationAdminFactory->notifyConfigurationDeleted(this, !factoryPid.isEmpty());
    configurationAdminFactory->dispatchEvent(ctkConfigurationEvent::CM_DELETED, factoryPid, pid);
  }
  configurationStore->removeConfiguration(pid);
}

QString ctkConfigurationImpl::getPluginLocation() const
{
  return getPluginLocation(true);
}

QString ctkConfigurationImpl::getFactoryPid() const
{
  return getFactoryPid(true);
}

QString ctkConfigurationImpl::getPid() const
{
  return getPid(true);
}

ctkDictionary ctkConfigurationImpl::getProperties() const
{
  ctkConfigurationImplLocker l(this);
  checkDeleted();
  if (dictionary.isEmpty())
      return dictionary;

  ctkDictionary copy = dictionary;
  copy.insert(ctkPluginConstants::SERVICE_PID, pid);
  if (!factoryPid.isEmpty())
  {
    copy.insert(ctkConfigurationAdmin::SERVICE_FACTORYPID, factoryPid);
  }

  return copy;
}

void ctkConfigurationImpl::setPluginLocation(const QString& pluginLocation)
{
  ctkConfigurationImplLocker l(this);
  checkDeleted();
  configurationAdminFactory->checkConfigurationPermission();
  this->pluginLocation = pluginLocation;
  boundPlugin.clear(); // always reset the boundPlugin when setPluginLocation is called
}

void ctkConfigurationImpl::update()
{
  ctkConfigurationImplLocker l(this);
  checkDeleted();
  configurationStore->saveConfiguration(pid, this);
  configurationAdminFactory->notifyConfigurationUpdated(this, !factoryPid.isEmpty());
}

void ctkConfigurationImpl::update(const ctkDictionary& properties)
{
  ctkConfigurationImplLocker l(this);
  checkDeleted();
  updateDictionary(properties);
  configurationStore->saveConfiguration(pid, this);
  configurationAdminFactory->notifyConfigurationUpdated(this, !factoryPid.isEmpty());
  configurationAdminFactory->dispatchEvent(ctkConfigurationEvent::CM_UPDATED, factoryPid, pid);
}

void ctkConfigurationImpl::lock() const
{
  QMutexLocker lock(&mutex);
  QThread* current = QThread::currentThread();
  if (lockHolder != current)
  {
    while (lockedCount != 0)
    {
      waitCond.wait(&mutex);
    }
  }
  ++lockedCount;
  lockHolder = current;
}

void ctkConfigurationImpl::unlock() const
{
  QMutexLocker lock(&mutex);
  QThread* current = QThread::currentThread();
  if (lockHolder != current)
  {
    throw ctkIllegalStateException("Thread not lock owner");
  }

  --lockedCount;
  if (lockedCount == 0)
  {
    lockHolder = 0;
    waitCond.wakeOne();
  }
}

void ctkConfigurationImpl::checkLocked() const
{
  QMutexLocker lock(&mutex);
  QThread* current = QThread::currentThread();
  if (lockHolder != current)
  {
    throw ctkIllegalStateException("Thread not lock owner");
  }
}

bool ctkConfigurationImpl::bind(QSharedPointer<ctkPlugin> plugin)
{
  ctkConfigurationImplLocker l(this);
  if (boundPlugin.isNull() && (pluginLocation.isEmpty() || pluginLocation == plugin->getLocation()))
  {
    boundPlugin = plugin;
  }
  return (boundPlugin == plugin);
}

void ctkConfigurationImpl::unbind(QSharedPointer<ctkPlugin> plugin)
{
  ctkConfigurationImplLocker l(this);
  if (boundPlugin == plugin)
  {
    boundPlugin.clear();
  }
}

QString ctkConfigurationImpl::getPluginLocation(bool checkPermission) const
{
  ctkConfigurationImplLocker l(this);
  checkDeleted();
  if (checkPermission)
  {
    configurationAdminFactory->checkConfigurationPermission();
  }

  if (!pluginLocation.isEmpty())
  {
    return pluginLocation;
  }

  if (boundPlugin)
  {
    return boundPlugin->getLocation();
  }

  return QString();
}

QString ctkConfigurationImpl::getFactoryPid(bool checkDel) const
{
  ctkConfigurationImplLocker l(this);
  if (checkDel)
  {
    checkDeleted();
  }
  return factoryPid;
}

QString ctkConfigurationImpl::getPid(bool checkDel) const
{
  ctkConfigurationImplLocker l(this);
  if (checkDel)
  {
    checkDeleted();
  }
  return pid;
}

ctkDictionary ctkConfigurationImpl::getAllProperties() const
{
  ctkConfigurationImplLocker l(this);
  if (deleted)
  {
    return ctkDictionary();
  }
  ctkDictionary copy = getProperties();
  if (copy.isEmpty())
  {
    return copy;
  }

  QString boundLocation = getPluginLocation(false);
  if (!boundLocation.isEmpty())
  {
    copy.insert(ctkConfigurationAdmin::SERVICE_PLUGINLOCATION, boundLocation);
  }
  return copy;
}

bool ctkConfigurationImpl::isDeleted() const
{
  ctkConfigurationImplLocker l(this);
  return deleted;
}

void ctkConfigurationImpl::checkDeleted() const
{
  if (deleted)
    throw ctkIllegalStateException("deleted");
}

void ctkConfigurationImpl::updateDictionary(const ctkDictionary& properties)
{
  ctkConfigurationDictionary newDictionary = properties;
  newDictionary.remove(ctkPluginConstants::SERVICE_PID);
  newDictionary.remove(ctkConfigurationAdmin::SERVICE_FACTORYPID);
  newDictionary.remove(ctkConfigurationAdmin::SERVICE_PLUGINLOCATION);
  dictionary = newDictionary;
}

ctkConfigurationImplLocker::ctkConfigurationImplLocker(const ctkConfigurationImpl* impl)
  : impl(impl)
{
  if (impl) impl->lock();
}

ctkConfigurationImplLocker::ctkConfigurationImplLocker(const QList<ctkConfigurationImplPtr>& implList)
  : impl(0), implList(implList)
{
  foreach(ctkConfigurationImplPtr i, this->implList)
  {
    if (i) i->lock();
  }
}

ctkConfigurationImplLocker::~ctkConfigurationImplLocker()
{
  if (impl) impl->unlock();
  foreach(ctkConfigurationImplPtr i, implList)
  {
    if (i) i->unlock();
  }
}
