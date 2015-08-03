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


#include "ctkConfigurationStore_p.h"
#include "ctkConfigurationAdminFactory_p.h"

#include <ctkPluginContext.h>
#include <service/log/ctkLogService.h>

#include <QDataStream>
#include <QDateTime>

const QString ctkConfigurationStore::STORE_DIR = "store";
const QString ctkConfigurationStore::PID_EXT = ".pid";

ctkConfigurationStore::ctkConfigurationStore(
  ctkConfigurationAdminFactory* configurationAdminFactory,
  ctkPluginContext* context)
  : configurationAdminFactory(configurationAdminFactory),
    createdPidCount(0)
{
  store = context->getDataFile(STORE_DIR).absoluteDir();

  if (!store.mkpath(store.absolutePath()))
  {
    return; // no persistent store
  }

  QStringList nameFilters;
  nameFilters << QString('*') + PID_EXT;
  QFileInfoList configurationFiles = store.entryInfoList(nameFilters, QDir::Files | QDir::CaseSensitive);
  foreach (QFileInfo configFileInfo, configurationFiles)
  {
    QString configurationFilePath = configFileInfo.absoluteFilePath();
    QString configurationFileName = configFileInfo.fileName();
    QString pid = configurationFileName.mid(0, configurationFileName.size() - PID_EXT.size());

    bool deleteFile = false;
    QIODevice* iodevice = new QFile(configurationFilePath);
    iodevice->open(QIODevice::ReadOnly);
    QDataStream dataStream(iodevice);

    ctkDictionary dictionary;
    dataStream >> dictionary;
    if (dataStream.status() == QDataStream::Ok)
    {
      ctkConfigurationImplPtr config(new ctkConfigurationImpl(configurationAdminFactory, this, dictionary));
      configurations.insert(config->getPid(), config);
    }
    else
    {
      QString message = iodevice->errorString();
      QString errorMessage = QString("{Configuration Admin - pid = %1} could not be restored. %2").arg(pid).arg(message);
      CTK_ERROR(configurationAdminFactory->getLogService()) << errorMessage;
      deleteFile = true;
    }

    iodevice->close();
    delete iodevice;

    if (deleteFile)
    {
      QFile::remove(configurationFilePath);
    }
  }
}

void ctkConfigurationStore::saveConfiguration(const QString& pid, ctkConfigurationImpl* config)
{
  if (!store.exists())
    return; // no persistent store

  config->checkLocked();
  QFile configFile(store.filePath(pid + PID_EXT));
  ctkDictionary configProperties = config->getAllProperties();
  //TODO security
//  try
//  {
//    AccessController.doPrivileged(new PrivilegedExceptionAction() {
//      public Object run() throws Exception {
        writeConfigurationFile(configFile, configProperties);
//        return null;
//      }
//    });
//  }
//  catch (PrivilegedActionException e)
//  {
//    throw (IOException) e.getException();
//  }
}

void ctkConfigurationStore::removeConfiguration(const QString& pid)
{
  QMutexLocker lock(&mutex);
  configurations.remove(pid);
  if (!store.exists())
    return; // no persistent store

  QFile configFile(store.filePath(pid + PID_EXT));
  //TODO security//  AccessController.doPrivileged(new PrivilegedAction() {
//    public Object run() {
  deleteConfigurationFile(configFile);
//      return null;
//    }
//  });
}

ctkConfigurationImplPtr ctkConfigurationStore::getConfiguration(
  const QString& pid, const QString& location)
{
  QMutexLocker lock(&mutex);
  ctkConfigurationImplPtr config = configurations.value(pid);
  if (config.isNull())
  {
    config = ctkConfigurationImplPtr(new ctkConfigurationImpl(configurationAdminFactory, this,
                                                              QString(), pid, location));
    configurations.insert(pid, config);
  }
  return config;
}

ctkConfigurationImplPtr ctkConfigurationStore::createFactoryConfiguration(
  const QString& factoryPid, const QString& location)
{
  QMutexLocker lock(&mutex);
  //TODO Qt4.7 use QDateTime::currentMSecsSinceEpoch()
  QString pid = factoryPid + "-" + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmsszzz") + "-" + QString::number(createdPidCount++);
  ctkConfigurationImplPtr config(new ctkConfigurationImpl(configurationAdminFactory, this, factoryPid, pid, location));
  configurations.insert(pid, config);
  return config;
}

ctkConfigurationImplPtr ctkConfigurationStore::findConfiguration(const QString& pid)
{
  QMutexLocker lock(&mutex);
  return configurations.value(pid);
}

QList<ctkConfigurationImplPtr> ctkConfigurationStore::getFactoryConfigurations(const QString& factoryPid)
{
  QMutexLocker lock(&mutex);
  QList<ctkConfigurationImplPtr> resultList;
  foreach (ctkConfigurationImplPtr config, configurations)
  {
    QString otherFactoryPid = config->getFactoryPid();
    if (otherFactoryPid == factoryPid)
    {
      resultList.push_back(config);
    }
  }
  return resultList;
}

QList<ctkConfigurationImplPtr> ctkConfigurationStore::listConfigurations(const ctkLDAPSearchFilter& filter)
{
  QMutexLocker lock(&mutex);
  QList<ctkConfigurationImplPtr> resultList;
  foreach (ctkConfigurationImplPtr config, configurations)
  {
    ctkDictionary properties = config->getAllProperties();
    if (filter.match(properties))
    {
      resultList.push_back(config);
    }
  }
  return resultList;
}

void ctkConfigurationStore::unbindConfigurations(QSharedPointer<ctkPlugin> plugin)
{
  QMutexLocker lock(&mutex);
  foreach (ctkConfigurationImplPtr config, configurations)
  {
    config->unbind(plugin);
  }
}

void ctkConfigurationStore::writeConfigurationFile(QFile& configFile,
                            const ctkDictionary& configProperties)
{
  QIODevice* iodevice = &configFile;
  iodevice->open(QIODevice::WriteOnly);
  QDataStream datastream(iodevice);

  datastream << configProperties;
  // ignore errors
  iodevice->close();
}

void ctkConfigurationStore::deleteConfigurationFile(QFile& configFile)
{
  configFile.remove();
}
