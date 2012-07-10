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


#include "ctkConfigurationListenerTestSuite_p.h"

#include <ctkPluginContext.h>

#include <service/cm/ctkConfiguration.h>
#include <service/cm/ctkConfigurationAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkConfigurationListenerTestSuite::ctkConfigurationListenerTestSuite(
  ctkPluginContext* pc, long cmPluginId)
  : context(pc), cm(0), locked(false), cmPluginId(cmPluginId)
{

}

//----------------------------------------------------------------------------
void ctkConfigurationListenerTestSuite::init()
{
  context->getPlugin(cmPluginId)->start();
  reference = context->getServiceReference<ctkConfigurationAdmin>();
  cm = context->getService<ctkConfigurationAdmin>(reference);
  listener = new _ConfigurationListenerTest(this);
}

//----------------------------------------------------------------------------
void ctkConfigurationListenerTestSuite::cleanup()
{
  context->ungetService(reference);
  context->getPlugin(cmPluginId)->stop();
  delete listener;
}

//----------------------------------------------------------------------------
void ctkConfigurationListenerTestSuite::testListener()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  ctkServiceRegistration reg = context->registerService<ctkConfigurationListener>(listener);

  {
    QMutexLocker l(&mutex);
    config->update(props);
    locked = true;
    QVERIFY2(lock.wait(&mutex, 5000), "Wait timed out!");
    QVERIFY(!locked);
  }

  reg.unregister();
  config->remove();
}

//----------------------------------------------------------------------------
_ConfigurationListenerTest::_ConfigurationListenerTest(
  ctkConfigurationListenerTestSuite* ts)
  : ts(ts)
{
}

//----------------------------------------------------------------------------
void _ConfigurationListenerTest::configurationEvent(
  const ctkConfigurationEvent& event)
{
  Q_UNUSED(event)

  {
    QMutexLocker lock(&ts->mutex);
    ts->locked = false;
  }
  ts->lock.wakeOne();
}
