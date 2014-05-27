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

#include "ctkEventAdminTestActivator_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <QtPlugin>
#include <QStringList>

#include "ctkEATopicWildcardTestSuite_p.h"
#include "ctkEAScenario1TestSuite_p.h"
#include "ctkEAScenario2TestSuite_p.h"
#include "ctkEAScenario3TestSuite_p.h"
#include "ctkEAScenario4TestSuite_p.h"

//----------------------------------------------------------------------------
ctkEventAdminTestActivator::ctkEventAdminTestActivator()
  : topicWildcardTestSuite(0)
  , topicWildcardTestSuiteSS(0)
  , scenario1TestSuite(0)
  , scenario1TestSuiteSS(0)
  , scenario2TestSuite(0)
  , scenario3TestSuite(0)
  , scenario4TestSuite(0)
{

}

//----------------------------------------------------------------------------
ctkEventAdminTestActivator::~ctkEventAdminTestActivator()
{
  delete topicWildcardTestSuite;
  delete topicWildcardTestSuiteSS;
  delete scenario1TestSuite;
  delete scenario1TestSuiteSS;
  delete scenario2TestSuite;
  delete scenario3TestSuite;
  delete scenario4TestSuite;
}

//----------------------------------------------------------------------------
void ctkEventAdminTestActivator::start(ctkPluginContext* context)
{
  QString symbolicName = context->getProperty("event.impl").toString();
  if (symbolicName.isEmpty())
  {
    throw ctkRuntimeException("Framework property 'event.impl' containing the symbolic "
                              "name of the EventAdmin implementation not found!");
  }

  long eventPluginId = -1;
  foreach(QSharedPointer<ctkPlugin> p, context->getPlugins())
  {
    if (p->getSymbolicName() == symbolicName)
    {
      eventPluginId = p->getPluginId();
      break;
    }
  }

  if (eventPluginId < 0)
  {
    QString msg = QString("The EventAdmin implementation '%1' is not installed.")
        .arg(symbolicName);
    throw ctkRuntimeException(msg);
  }

  topicWildcardTestSuite = new ctkEATopicWildcardTestSuite(context, eventPluginId, false);
  context->registerService<ctkTestSuiteInterface>(topicWildcardTestSuite);

  topicWildcardTestSuiteSS = new ctkEATopicWildcardTestSuite(context, eventPluginId, true);
  context->registerService<ctkTestSuiteInterface>(topicWildcardTestSuiteSS);

  scenario1TestSuite = new ctkEAScenario1TestSuite(context, eventPluginId, false);
  context->registerService<ctkTestSuiteInterface>(scenario1TestSuite);

  scenario1TestSuiteSS = new ctkEAScenario1TestSuite(context, eventPluginId, true);
  context->registerService<ctkTestSuiteInterface>(scenario1TestSuiteSS);

  scenario2TestSuite = new ctkEAScenario2TestSuite(context, eventPluginId);
  context->registerService<ctkTestSuiteInterface>(scenario2TestSuite);

  scenario3TestSuite = new ctkEAScenario3TestSuite(context, eventPluginId);
  context->registerService<ctkTestSuiteInterface>(scenario3TestSuite);

  scenario4TestSuite = new ctkEAScenario4TestSuite(context, eventPluginId);
  context->registerService<ctkTestSuiteInterface>(scenario4TestSuite);
}

//----------------------------------------------------------------------------
void ctkEventAdminTestActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);

  delete topicWildcardTestSuite;
  delete topicWildcardTestSuiteSS;
  delete scenario1TestSuite;
  delete scenario1TestSuiteSS;
  delete scenario2TestSuite;
  delete scenario3TestSuite;
  delete scenario4TestSuite;

  topicWildcardTestSuite = 0;
  topicWildcardTestSuiteSS = 0;
  scenario1TestSuite = 0;
  scenario1TestSuiteSS = 0;
  scenario2TestSuite = 0;
  scenario3TestSuite = 0;
  scenario4TestSuite = 0;
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(org_commontk_eventadmintest, ctkEventAdminTestActivator)
#endif
