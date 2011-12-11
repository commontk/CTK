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


#include "ctkManagedServiceFactoryTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <service/cm/ctkConfiguration.h>
#include <service/cm/ctkConfigurationAdmin.h>

#include <QTest>

//----------------------------------------------------------------------------
_ManagedServiceFactoryUpdateTest::_ManagedServiceFactoryUpdateTest(ctkManagedServiceFactoryTestSuite* ts)
  : ts(ts)
{

}

//----------------------------------------------------------------------------
void _ManagedServiceFactoryUpdateTest::deleted(const QString& pid)
{
  Q_UNUSED(pid)

  QMutexLocker l(&ts->mutex);
  ts->locked = false;
  ts->lock.wakeOne();
  ts->updateCount++;
}

//----------------------------------------------------------------------------
QString _ManagedServiceFactoryUpdateTest::getName()
{
  return QString();
}

//----------------------------------------------------------------------------
void _ManagedServiceFactoryUpdateTest::updated(const QString& pid, const ctkDictionary& properties)
{
  Q_UNUSED(pid)
  Q_UNUSED(properties)

  QMutexLocker l(&ts->mutex);
  ts->locked = false;
  ts->lock.wakeOne();
  ts->updateCount++;
}

//----------------------------------------------------------------------------
ctkManagedServiceFactoryTestSuite::ctkManagedServiceFactoryTestSuite(ctkPluginContext* pc, long cmPluginId)
  : context(pc), cmPluginId(cmPluginId), cm(0), updateCount(0),
    locked(false)
{

}

//----------------------------------------------------------------------------
void ctkManagedServiceFactoryTestSuite::init()
{
  context->getPlugin(cmPluginId)->start();
  reference = context->getServiceReference<ctkConfigurationAdmin>();
  cm = context->getService<ctkConfigurationAdmin>(reference);
}

//----------------------------------------------------------------------------
void ctkManagedServiceFactoryTestSuite::cleanup()
{
  context->ungetService(reference);
  context->getPlugin(cmPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkManagedServiceFactoryTestSuite::testSamePidManagedServiceFactory()
{
  ctkConfigurationPtr config = cm->createFactoryConfiguration("test");
  ctkDictionary props;
  props.insert("testkey", "testvalue");
  config->update(props);

  updateCount = 0;
  _ManagedServiceFactoryUpdateTest msf(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");
  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    reg = context->registerService<ctkManagedServiceFactory>(&msf, dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(1, updateCount);
  }

  ctkServiceRegistration reg2;
  {
    QMutexLocker l(&mutex);
    reg2 = context->registerService<ctkManagedServiceFactory>(&msf, dict);
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
void ctkManagedServiceFactoryTestSuite::testGeneralManagedServiceFactory()
{
  updateCount = 0;
  _ManagedServiceFactoryUpdateTest msf(this);

  ctkDictionary dict;
  dict.insert(ctkPluginConstants::SERVICE_PID, "test");

  ctkServiceRegistration reg;
  {
    QMutexLocker l(&mutex);
    reg = context->registerService<ctkManagedServiceFactory>(&msf, dict);
    locked = true;
    lock.wait(&mutex, 100);
    QVERIFY(locked);
    QCOMPARE(0, updateCount);
    locked = false;
  }

  ctkConfigurationPtr config = cm->createFactoryConfiguration("test");
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
    QCOMPARE(1, updateCount);
  }

  dict.remove(ctkPluginConstants::SERVICE_PID);
  {
    QMutexLocker l(&mutex);
    reg.setProperties(dict);
    props.insert("testkey", "testvalue2");
    config->update(props);
    locked = true;
    lock.wait(&mutex, 100);
    QVERIFY(locked);
    QCOMPARE(1, updateCount);
    locked = false;
  }

  config->remove();
  config = cm->createFactoryConfiguration("test2");
  dict.insert(ctkPluginConstants::SERVICE_PID, "test2");
  {
    QMutexLocker l(&mutex);
    reg.setProperties(dict);
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(2, updateCount);
  }

  {
    QMutexLocker l(&mutex);
    config->remove();
    locked = true;
    lock.wait(&mutex, 5000);
    if (locked)
      QFAIL("should have updated");
    QCOMPARE(3, updateCount);
  }
  reg.unregister();
}
