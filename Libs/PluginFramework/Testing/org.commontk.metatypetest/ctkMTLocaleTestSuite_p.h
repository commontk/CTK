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


#ifndef CTKMTLOCALETESTSUITE_P_H
#define CTKMTLOCALETESTSUITE_P_H

#include <QObject>

#include <ctkServiceReference.h>
#include <ctkTestSuiteInterface.h>

class ctkPluginContext;
struct ctkMetaTypeService;

class ctkMTLocaleTestSuite : public QObject,
    public ctkTestSuiteInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkTestSuiteInterface)

public:

  ctkMTLocaleTestSuite(ctkPluginContext* pc, long mtPluginId);

private Q_SLOTS:

  void initTestCase();
  void cleanupTestCase();

  /*
   *
   */
  void testLocaleList();

private:

  QSharedPointer<ctkPlugin> plugin;
  ctkPluginContext* context;
  long mtPluginId;
  ctkMetaTypeService* mts;
  ctkServiceReference reference;
};

#endif // CTKMTLOCALETESTSUITE_P_H
