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


#ifndef CTKCONFIGURATIONIMPL_P_H
#define CTKCONFIGURATIONIMPL_P_H

#include <service/cm/ctkConfiguration.h>

#include <QMutex>
#include <QWaitCondition>

class ctkConfigurationAdminFactory;
class ctkConfigurationStore;
class ctkPlugin;

/**
 * ctkConfigurationImpl provides the ctkConfiguration implementation.
 * The lock and unlock methods are used for synchronization. Operations outside of
 * ConfigurationImpl that expect to have control of the lock should call checkLocked
 */
class ctkConfigurationImpl : public ctkConfiguration
{

public:

  ctkConfigurationImpl(ctkConfigurationAdminFactory* configurationAdminFactory,
                       ctkConfigurationStore* configurationStore,
                       const QString& factoryPid, const QString& pid,
                       const QString& pluginLocation);

  ctkConfigurationImpl(ctkConfigurationAdminFactory* configurationAdminFactory,
                       ctkConfigurationStore* configurationStore,
                       const ctkDictionary& dictionary);

  void remove();

  QString getPluginLocation() const;
  QString getFactoryPid() const;
  QString getPid() const;
  ctkDictionary getProperties() const;

  void setPluginLocation(const QString& pluginLocation);

  void update();
  void update(const ctkDictionary& properties);

  void checkLocked() const;

  bool bind(QSharedPointer<ctkPlugin> plugin);
  void unbind(QSharedPointer<ctkPlugin> plugin);

  QString getPluginLocation(bool checkPermission) const;
  QString getFactoryPid(bool checkDeleted) const;
  QString getPid(bool checkDeleted) const;
  ctkDictionary getAllProperties() const;

  void lock() const;
  void unlock() const;

  bool isDeleted() const;

private:

  typedef ctkDictionary ctkConfigurationDictionary;

  mutable QMutex mutex;
  mutable QWaitCondition waitCond;

  ctkConfigurationAdminFactory* configurationAdminFactory;
  ctkConfigurationStore* configurationStore;
  /** @GuardedBy mutex*/
  QString pluginLocation;
  QString factoryPid;
  QString pid;
  ctkConfigurationDictionary dictionary;
  /** @GuardedBy mutex*/
  bool deleted;
  /** @GuardedBy mutex*/
  QSharedPointer<ctkPlugin> boundPlugin;
  /** @GuardedBy mutex*/
  mutable int lockedCount;
  /** @GuardedBy mutex*/
  mutable QThread* lockHolder;

  void checkDeleted() const;

  void updateDictionary(const ctkDictionary& properties);

};

typedef QSharedPointer<ctkConfigurationImpl> ctkConfigurationImplPtr;

class ctkConfigurationImplLocker
{
public:

  ctkConfigurationImplLocker(const ctkConfigurationImpl* impl);
  ctkConfigurationImplLocker(const QList<ctkConfigurationImplPtr>& implList);
  ~ctkConfigurationImplLocker();

private:

  const ctkConfigurationImpl* impl;
  QList<ctkConfigurationImplPtr> implList;
};

#endif // CTKCONFIGURATIONIMPL_P_H
