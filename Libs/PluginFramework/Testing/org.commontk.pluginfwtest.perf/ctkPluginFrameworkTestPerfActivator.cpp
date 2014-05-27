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

#include "ctkPluginFrameworkTestPerfActivator_p.h"

#include "ctkPluginFrameworkPerfRegistryTestSuite_p.h"

#include <QtPlugin>


//----------------------------------------------------------------------------
ctkPluginFrameworkTestPerfActivator::ctkPluginFrameworkTestPerfActivator()
  : perfTestSuite(0)
{

}

//----------------------------------------------------------------------------
ctkPluginFrameworkTestPerfActivator::~ctkPluginFrameworkTestPerfActivator()
{
  delete perfTestSuite;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestPerfActivator::start(ctkPluginContext* context)
{
  perfTestSuite = new ctkPluginFrameworkPerfRegistryTestSuite(context);
  context->registerService<ctkTestSuiteInterface>(perfTestSuite);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkTestPerfActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);

  delete perfTestSuite;
  perfTestSuite = 0;
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(org_commontk_pluginfwtest_perf, ctkPluginFrameworkTestPerfActivator)
#endif
