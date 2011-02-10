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


#include "ctkMTAttrPasswordTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>
#include <ctkPluginFrameworkTestUtil.h>

#include <service/metatype/ctkMetaTypeInformation.h>
#include <service/metatype/ctkMetaTypeService.h>

#include <QTest>
#include <QDebug>

//----------------------------------------------------------------------------
ctkMTAttrPasswordTestSuite::ctkMTAttrPasswordTestSuite(
  ctkPluginContext* pc, long mtPluginId)
  : context(pc), mtPluginId(mtPluginId), mts(0)
{

}

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::initTestCase()
{
  context->getPlugin(mtPluginId)->start();
  reference = context->getServiceReference<ctkMetaTypeService>();
  mts = context->getService<ctkMetaTypeService>(reference);
  plugin = ctkPluginFrameworkTestUtil::installPlugin(context, "pluginAttrPwd_test");
  plugin->start();
}

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::cleanupTestCase()
{
  context->ungetService(reference);
  context->getPlugin(mtPluginId)->stop();
}

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::testAttributeTypePassword1()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd");
  QVERIFY(ocd);
  QList<ctkAttributeDefinitionPtr> ads = ocd->getAttributeDefinitions(ctkObjectClassDefinition::ALL);
  for (int i = 0; i < ads.size(); i++)
  {
    if (ads[i]->getID() == "password1")
    {
      QVERIFY2(ctkAttributeDefinition::PASSWORD == ads[i]->getType(),
               "Attribute type is not PASSWORD");
    }
  }
}

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::testAttributeTypePassword2()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd");
  QVERIFY(ocd);
  QList<ctkAttributeDefinitionPtr> ads = ocd->getAttributeDefinitions(ctkObjectClassDefinition::ALL);
  for (int i = 0; i < ads.size(); i++)
  {
    if (ads[i]->getID() == "password1")
    {
      QVERIFY2(ads[i]->validate("1234abcd").isNull(), "Validation should not be present when min and max are not specified and there are no enumerated constraints");
    }
  }
 }

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::testAttributeTypePassword3()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd");
  QList<ctkAttributeDefinitionPtr> ads = ocd->getAttributeDefinitions(ctkObjectClassDefinition::ALL);
  for (int i = 0; i < ads.size(); i++)
  {
    if (ads[i]->getID() == "password2")
    {
      QVERIFY2(!ads[i]->validate("password").isNull(),
               "Validation should be present when min and max are not specified and their are enumerated constraints");
      QVERIFY2(0 == ads[i]->validate("password").size(), "Value 'password' should have been valid");
      QVERIFY2(ads[i]->validate("1234abcd").size() > 0, "Value '1234abcd' should not have been valid");
    }
  }
}

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::testAttributeTypePassword4()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd");
  QList<ctkAttributeDefinitionPtr> ads = ocd->getAttributeDefinitions(ctkObjectClassDefinition::ALL);
  for (int i = 0; i < ads.size(); i++)
  {
    if (ads[i]->getID() == "password3")
    {
      QVERIFY2(0 == ads[i]->validate("12345678").size(), "Value '12345678' should have been valid");
      QVERIFY2(0 == ads[i]->validate("123456789").size(), "Value '123456789' should have been valid");
      QVERIFY2(ads[i]->validate("1234567").size() > 0, "Value '1234567' should not have been valid");
   }
  }
 }

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::testAttributeTypePassword5()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd");
  QList<ctkAttributeDefinitionPtr> ads = ocd->getAttributeDefinitions(ctkObjectClassDefinition::ALL);
  for (int i = 0; i < ads.size(); i++)
  {
    if (ads[i]->getID() == "password4")
    {
      QVERIFY2(0 == ads[i]->validate("12345").size(), "Value '12345' should have been valid");
      QVERIFY2(0 == ads[i]->validate("1234").size(), "Value '1234' should have been valid");
      QVERIFY2(ads[i]->validate("123456").size() > 0, "Value '123456' should not have been valid");
    }
  }
}

//----------------------------------------------------------------------------
void ctkMTAttrPasswordTestSuite::testAttributeTypePassword6()
{
  ctkMetaTypeInformationPtr mti = mts->getMetaTypeInformation(plugin);
  ctkObjectClassDefinitionPtr ocd = mti->getObjectClassDefinition("org.commontk.metatype.tests.attrpwd");
  QList<ctkAttributeDefinitionPtr> ads = ocd->getAttributeDefinitions(ctkObjectClassDefinition::ALL);
  for (int i = 0; i < ads.size(); i++)
  {
    if (ads[i]->getID() == "password5")
    {
      QVERIFY2(0 == ads[i]->validate("123").size(), "Value '123' should have been valid");
      QVERIFY2(ads[i]->validate("12").size() > 0, "Value '12' should not have been valid");
      QVERIFY2(0 == ads[i]->validate("123456").size(), "Value '123456' should have been valid");
      QVERIFY2(ads[i]->validate("1234567").size() > 0, "Value '1234567' should not have been valid");
   }
  }
 }

