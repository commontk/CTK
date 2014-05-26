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

#include "ctkEventAdminTestPerfActivator_p.h"

#include "ctkEventAdminPerfTestSuite_p.h"

#include <QtPlugin>


//----------------------------------------------------------------------------
ctkEventAdminTestPerfActivator::ctkEventAdminTestPerfActivator()
  : perfTestSuite(0)
{

}

//----------------------------------------------------------------------------
ctkEventAdminTestPerfActivator::~ctkEventAdminTestPerfActivator()
{
  delete perfTestSuite;
}

//----------------------------------------------------------------------------
void ctkEventAdminTestPerfActivator::start(ctkPluginContext* context)
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

  perfTestSuite = new ctkEventAdminPerfTestSuite(context, eventPluginId);
  context->registerService<ctkTestSuiteInterface>(perfTestSuite);
}

//----------------------------------------------------------------------------
void ctkEventAdminTestPerfActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);

  delete perfTestSuite;
  perfTestSuite = 0;
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(org_commontk_eventadmintest_perf, ctkEventAdminTestPerfActivator)
#endif
