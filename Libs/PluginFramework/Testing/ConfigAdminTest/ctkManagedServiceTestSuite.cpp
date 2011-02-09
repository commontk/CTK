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


#include "ctkManagedServiceTestSuite_p.h"

#include <service/cm/ctkConfigurationAdmin.h>
#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <QTest>

//----------------------------------------------------------------------------
_ManagedServiceUpdateTest::_ManagedServiceUpdateTest(
  ctkManagedServiceTestSuite* ts)
  : ts(ts)
{

}

//----------------------------------------------------------------------------
void _ManagedServiceUpdateTest::updated(
  const ctkDictionary& properties)
{
  Q_UNUSED(properties)

  QMutexLocker l(&ts->mutex);
  ts->locked = false;
  ts->lock.wakeOne();
  ts->updateCount++;
}

//----------------------------------------------------------------------------
ctkManagedServiceTestSuite::ctkManagedServiceTestSuite(
  ctkPluginContext* pc, long cmPluginId)
  : context(pc), cmPluginId(cmPluginId), cm(0), updateCount(0),
    locked(false)
{

}

//----------------------------------------------------------------------------
void ctkManagedServiceTestSuite::init()
{
  context->getPlugin(cmPluginId)->start();
  reference = context->getServiceReference<ctkConfigurationAdmin>();
  cm = context->getService<ctkConfigurationAdmin>(reference);
}

//----------------------------------------------------------------------------
void ctkManagedServiceTestSuite::cleanup()
{
  context->ungetService(reference);
  context->getPlugin(cmPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkManagedServiceTestSuite::testSamePidManagedService()
{
  ctkConfigurationPtr config = cm->getConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  updateCount = 0;
  _ManagedServiceUpdateTest ms(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");
  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    reg = context->registerService<ctkManagedService>(&ms, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(1, updateCount);
  }

  ctkServiceRegistration reg2;
  {
    QMutexLocker l(&mutex);
    reg2 = context->registerService<ctkManagedService>(&ms, dict);
    locked = true;
    lock.wait(&mutex, 100);
    QVERIFY(locked);
    QCOMPARE(1, updateCount);
    locked = false;
  }
  reg.unregister();
  reg2.unregister();
  config->remove();
}

//----------------------------------------------------------------------------
void ctkManagedServiceTestSuite::testGeneralManagedService()
{
  updateCount = 0;
  _ManagedServiceUpdateTest ms(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");

  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    reg = context->registerService<ctkManagedService>(&ms, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(1, updateCount);
  }

  ctkConfigurationPtr config = cm->getConfiguration("test");
  QVERIFY(config->getProperties().isEmpty());
  ctkDictionary props;
  props.insert("testkey", "testvalue");

  {
    QMutexLocker l(&mutex);
    config->update(props);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(2, updateCount);
  }

  QString location = config->getPluginLocation();
  config->setPluginLocation("bogus");
  {
    QMutexLocker l(&mutex);
    config->update();
    locked = true;
    lock.wait(&mutex, 100);
    QVERIFY(locked);
    QCOMPARE(2, updateCount);
    locked = false;
  }
  config->setPluginLocation(location);

  dict.remove(ctkPluginConstants::SERVICE_PID);
  {
    QMutexLocker l(&mutex);
    reg.setProperties(dict);
    props.insert("testkey", "testvalue2");
    config->update(props);
    locked = true;
    lock.wait(&mutex, 100);
    QVERIFY(locked);
    QCOMPARE(2, updateCount);
    locked = false;
  }

  config->remove();
  config = cm->getConfiguration("test2");
  dict.insert(ctkPluginConstants::SERVICE_PID, "test2");
  {
    QMutexLocker l(&mutex);
    reg.setProperties(dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(3, updateCount);
  }

  {
    QMutexLocker l(&mutex);
    config->remove();
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(4, updateCount);
  }
  reg.unregister();
}
