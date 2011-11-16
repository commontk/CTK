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


#ifndef CTKMTATTRPASSWORDTESTSUITE_P_H
#define CTKMTATTRPASSWORDTESTSUITE_P_H

#include <QObject>

#include <ctkServiceReference.h>
#include <ctkTestSuiteInterface.h>

class ctkPluginContext;
struct ctkMetaTypeService;

class ctkMTAttrPasswordTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkMTAttrPasswordTestSuite(ctkPluginContext* pc, long mtPluginId);

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  /*
   * Ensures the PASSWORD type is recognized.
   */
  void testAttributeTypePassword1();

  /*
   * Ensures the PASSWORD type is treated the same as the STRING type.
   * Validation should not be present when min and max are not specified and
   * their are no enumerated constraints.
   */
  void testAttributeTypePassword2();

  /*
   * Ensures the PASSWORD type is treated the same as the STRING type.
   * Validation should be present when min and max are not specified and
   * their are enumerated constraints.
   */
  void testAttributeTypePassword3();

  /*
   * Ensures the PASSWORD type is treated the same as the STRING type.
   * PASSWORD length should be no less than min.
   */
  void testAttributeTypePassword4();

  /*
   * Ensures the PASSWORD type is treated the same as the STRING type.
   * PASSWORD length should be no greater than max.
   */
  void testAttributeTypePassword5();

  /*
   * Ensures the PASSWORD type is treated the same as the STRING type.
   * PASSWORD length should be no less than min and no greater than max.
   */
  void testAttributeTypePassword6();

private:

  QSharedPointer<ctkPlugin> plugin;
  ctkPluginContext* context;
  long mtPluginId;
  ctkMetaTypeService* mts;
  ctkServiceReference reference;
};

#endif // CTKMTATTRPASSWORDTESTSUITE_P_H
