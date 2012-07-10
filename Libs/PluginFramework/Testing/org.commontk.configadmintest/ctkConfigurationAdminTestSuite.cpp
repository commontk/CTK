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


#include "ctkConfigurationAdminTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>
#include <service/cm/ctkConfigurationAdmin.h>
#include <service/cm/ctkConfiguration.h>

#include <QTest>
#include <QDebug>

//----------------------------------------------------------------------------
ctkConfigurationAdminTestSuite::ctkConfigurationAdminTestSuite(
  ctkPluginContext* pc, long cmPluginId)
  : context(pc), cmPluginId(cmPluginId), cm(0)
{

}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::init()
{
  context->getPlugin(cmPluginId)->start();
  reference = context->getServiceReference<ctkConfigurationAdmin>();
  cm = context->getService<ctkConfigurationAdmin>(reference);
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::cleanup()
{
  context->ungetService(reference);
  context->getPlugin(cmPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateConfig()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  QCOMPARE(QString("test"), config->getPid());
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateConfigNullPid()
{
  try
  {
    cm->getConfiguration(QString());
  }
  catch (const ctkException& )
  {
    return;
  }
  QFAIL("no exception thrown");
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateConfigWithLocation()
{
  ctkConfigurationPtr config = cm->getConfiguration("test", QString());
  QCOMPARE(QString("test"), config->getPid());
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateConfigNullPidWithLocation()
{
  try
  {
    cm->getConfiguration(QString(), QString());
  }
  catch (const ctkException& )
  {
    return;
  }
  QFAIL("no exception thrown");
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateConfigWithAndWithoutLocation()
{
  ctkConfigurationPtr config = cm->getConfiguration("test", "x");
  config->update();
  ctkConfigurationPtr config2 = cm->getConfiguration("test");
  QCOMPARE(config, config2);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateConfigWithAndWithoutNullLocation()
{
  ctkConfigurationPtr config = cm->getConfiguration("test", QString());
  config->update();
  QVERIFY(config->getPluginLocation().isEmpty());
  ctkConfigurationPtr config2 = cm->getConfiguration("test");
  QCOMPARE(config, config2);
  QCOMPARE(config2->getPluginLocation(), context->getPlugin()->getLocation());
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateFactoryConfig()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test");
  QCOMPARE(QString("test"), config->getFactoryPid());
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateFactoryConfigNullPid()
{
  try
  {
    cm->createFactoryConfiguration(QString());
  }
  catch (const ctkException& )
  {
    return;
  }
  QFAIL("no exception thrown");
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateFactoryConfigWithLocation()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test", QString());
  QCOMPARE(QString("test"), config->getFactoryPid());
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateFactoryConfigNullPidWithLocation()
{
  try
  {
    cm->createFactoryConfiguration(QString(), QString());
  }
  catch (const ctkException& )
  {
    return;
  }
  QFAIL("no exception thrown");
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateFactoryConfigWithAndWithoutLocation()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test", "x");
  config->update();
  ctkConfigurationPtr config2 = cm->getConfiguration(config->getPid());
  QCOMPARE(config, config2);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testCreateFactoryConfigWithAndWithoutNullLocation()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test", QString());
  config->update();
  QVERIFY(config->getPluginLocation().isEmpty());
  ctkConfigurationPtr config2 = cm->getConfiguration(config->getPid());
  QCOMPARE(config, config2);
  QCOMPARE(config2->getPluginLocation(), context->getPlugin()->getLocation());
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testListConfiguration()
{
  ctkConfigurationPtr config = cm->getConfiguration("test", QString());
  qDebug() << "$$$$ config props:" << config->getProperties();
  config->update();
  QList<ctkConfigurationPtr> configs = cm->listConfigurations(QString("(") + ctkPluginConstants::SERVICE_PID + "=test)");
  QVERIFY(configs.isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  configs = cm->listConfigurations(QString("(") + ctkPluginConstants::SERVICE_PID + "=test)");
  QVERIFY(configs.size() > 0);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testListConfigurationWithBoundLocation()
{
  ctkConfigurationPtr config = cm->getConfiguration("test", QString());
  config->update();
  QString filterString = QString("(&(") + ctkConfigurationAdmin::SERVICE_PLUGINLOCATION
      + "=" + context->getPlugin()->getLocation() + ")(" + ctkPluginConstants::SERVICE_PID + "=test)" + ")";
  QList<ctkConfigurationPtr> configs = cm->listConfigurations(filterString);
  QVERIFY(configs.isEmpty());
  // bind ctkConfigurationPtr to this plugin's location
  cm->getConfiguration("test");
  configs = cm->listConfigurations(filterString);
  QVERIFY(configs.isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  configs = cm->listConfigurations(filterString);
  QVERIFY(configs.size() > 0);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testListFactoryConfiguration()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test", QString());
  config->update();
  QString filterString = QString("(") + ctkConfigurationAdmin::SERVICE_FACTORYPID + "=test)";
  QList<ctkConfigurationPtr> configs = cm->listConfigurations(filterString);
  QVERIFY(configs.isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  configs = cm->listConfigurations(filterString);
  QVERIFY(configs.size() > 0);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testListFactoryConfigurationWithBoundLocation()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test", QString());
  config->update();
  QString filterString = QString("(&(") + ctkConfigurationAdmin::SERVICE_PLUGINLOCATION
      + "=" + context->getPlugin()->getLocation() + ")(" + ctkPluginConstants::SERVICE_PID + "="
      + config->getPid() + "))";
  QList<ctkConfigurationPtr> configs = cm->listConfigurations(filterString);
  QVERIFY(configs.isEmpty());
  // bind ctkConfigurationPtr to this plugin's location
  cm->getConfiguration(config->getPid());
  configs = cm->listConfigurations(filterString);
  QVERIFY(configs.isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  configs = cm->listConfigurations(filterString);
  QVERIFY(configs.size() > 0);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testListConfigurationNull()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test", QString());
  config->update();
  QList<ctkConfigurationPtr> configs = cm->listConfigurations(QString());
  QVERIFY(configs.isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  configs = cm->listConfigurations(QString());
  QVERIFY(configs.size() > 0);
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testPersistentConfig()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  QVERIFY(config->getProperties().isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  QVERIFY(config->getPid() == "test");
  QVERIFY(config->getProperties().value("testkey").toString() == "testvalue");
  cleanup();
  init();
  config = cm->getConfiguration("test");
  QVERIFY(config->getProperties().value("testkey").toString() == "testvalue");
  config->remove();
  cleanup();
  init();
  config = cm->getConfiguration("test");
  QVERIFY(config->getProperties().isEmpty());
}

//----------------------------------------------------------------------------
void ctkConfigurationAdminTestSuite::testPersistentFactoryConfig()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test");
  QVERIFY(config->getProperties().isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);
  QCOMPARE(config->getFactoryPid(), QString("test"));
  QCOMPARE(config->getProperties().value("testkey").toString(), QString("testvalue"));
  QString pid = config->getPid();
  cleanup();
  init();
  config = cm->getConfiguration(pid);
  QCOMPARE(config->getProperties().value("testkey").toString(), QString("testvalue"));
  config->remove();
  cleanup();
  init();
  config = cm->getConfiguration(pid);
  QVERIFY(config->getProperties().isEmpty());
}
