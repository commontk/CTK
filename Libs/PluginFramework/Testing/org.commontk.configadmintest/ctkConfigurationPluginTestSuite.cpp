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


#include "ctkConfigurationPluginTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <service/cm/ctkConfiguration.h>
#include <service/cm/ctkConfigurationAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
_ConfigurationPluginTest::_ConfigurationPluginTest(const QString& prop)
  : prop(prop)
{

}

//----------------------------------------------------------------------------
void _ConfigurationPluginTest::modifyConfiguration(const ctkServiceReference& reference,
                         ctkDictionary& properties)
{
  Q_UNUSED(reference)
  properties.insert("plugin", prop);
}

//----------------------------------------------------------------------------
_ManagedServiceCMPluginTest::_ManagedServiceCMPluginTest(ctkConfigurationPluginTestSuite* ts)
  : ts(ts)
{

}

//----------------------------------------------------------------------------
void _ManagedServiceCMPluginTest::updated(const ctkDictionary& properties)
{
  QMutexLocker l(&ts->mutex);
  ts->locked = false;
  ts->lock.wakeOne();
  ts->success = properties.value("plugin").toString() == "plugin1";
}

//----------------------------------------------------------------------------
ctkConfigurationPluginTestSuite::ctkConfigurationPluginTestSuite(
  ctkPluginContext* pc, long cmPluginId)
  : context(pc), cmPluginId(cmPluginId), cm(0), locked(false),
    success(false)
{

}

//----------------------------------------------------------------------------
ctkConfigurationPluginTestSuite::~ctkConfigurationPluginTestSuite()
{
}

//----------------------------------------------------------------------------
void ctkConfigurationPluginTestSuite::init()
{
  context->getPlugin(cmPluginId)->start();
  reference = context->getServiceReference<ctkConfigurationAdmin>();
  cm = context->getService<ctkConfigurationAdmin>(reference);
}

//----------------------------------------------------------------------------
void ctkConfigurationPluginTestSuite::cleanup()
{
  context->ungetService(reference);
  context->getPlugin(cmPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkConfigurationPluginTestSuite::testPlugin()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  _ConfigurationPluginTest configPlugin;
  ctkServiceRegistration pluginReg = context->registerService<ctkConfigurationPlugin>(&configPlugin);

  _ManagedServiceCMPluginTest managedService(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");
  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    success = false;
    reg = context->registerService<ctkManagedService>(&managedService, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QVERIFY(success);
  }

  reg.unregister();
  pluginReg.unregister();
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationPluginTestSuite::testPidSpecificPlugin()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  _ConfigurationPluginTest configPlugin;
  ctkDictionary pluginDict;
  pluginDict.insert(ctkConfigurationPlugin::CM_TARGET, "test");
  ctkServiceRegistration pluginReg = context->registerService<ctkConfigurationPlugin>(&configPlugin, pluginDict);

  _ManagedServiceCMPluginTest ms(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");
  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    success = false;
    reg = context->registerService<ctkManagedService>(&ms, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QVERIFY(success);
  }

  reg.unregister();
  pluginReg.unregister();
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationPluginTestSuite::testPidSpecificMissPlugin()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  _ConfigurationPluginTest configPlugin;
  ctkDictionary pluginDict;
  pluginDict.insert(ctkConfigurationPlugin::CM_TARGET, "testXXX");
  ctkServiceRegistration pluginReg = context->registerService<ctkConfigurationPlugin>(&configPlugin, pluginDict);

  _ManagedServiceCMPluginTest ms(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");
  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    success = false;
    reg = context->registerService<ctkManagedService>(&ms, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    QVERIFY(!success);
  }

  reg.unregister();
  pluginReg.unregister();
  config->remove();
}

//----------------------------------------------------------------------------
void ctkConfigurationPluginTestSuite::testRankedPlugin()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  _ConfigurationPluginTest configPlugin;
  ctkDictionary pluginDict;
  pluginDict.insert(ctkConfigurationPlugin::CM_RANKING, 1);
  ctkServiceRegistration pluginReg1 = context->registerService<ctkConfigurationPlugin>(&configPlugin, pluginDict);

  _ConfigurationPluginTest configPlugin2("plugin2");

  ctkServiceRegistration pluginReg2 = context->registerService<ctkConfigurationPlugin>(&configPlugin2);

  _ManagedServiceCMPluginTest ms(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");
  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    success = false;
    reg = context->registerService<ctkManagedService>(&ms, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QVERIFY(success);
  }

 reg.unregister();
 pluginReg1.unregister();
 pluginReg2.unregister();
 config->remove();
}
