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


#include "ctkMTLocaleTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>
#include <ctkPluginFrameworkTestUtil.h>

#include <service/metatype/ctkMetaTypeInformation.h>
#include <service/metatype/ctkMetaTypeService.h>

#include <QTest>
#include <QDebug>

//----------------------------------------------------------------------------
ctkMTLocaleTestSuite::ctkMTLocaleTestSuite(
  ctkPluginContext* pc, long mtPluginId)
  : context(pc), mtPluginId(mtPluginId), mts(0)
{

}

//----------------------------------------------------------------------------
void ctkMTLocaleTestSuite::initTestCase()
{
  context->getPlugin(mtPluginId)->start();
  reference = context->getServiceReference<ctkMetaTypeService>();
  mts = context->getService<ctkMetaTypeService>(reference);
  plugin = ctkPluginFrameworkTestUtil::installPlugin(context, "pluginAttrPwd_test");
  plugin->start();
}

//----------------------------------------------------------------------------
void ctkMTLocaleTestSuite::cleanupTestCase()
{
  context->ungetService(reference);
  context->getPlugin(mtPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkMTLocaleTestSuite::testLocaleList()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  QList<QLocale> locales = mti->getLocales();
  QVERIFY2(locales.size() == 1, "Should have found one locale");
  QCOMPARE(locales[0].name(), QString("de_DE"));
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd", locales[0]);
  QCOMPARE(ocd->getName(), QString("Objekt"));
  QCOMPARE(ocd->getDescription(), QString("Meine Objektklassendefinition"));
}

