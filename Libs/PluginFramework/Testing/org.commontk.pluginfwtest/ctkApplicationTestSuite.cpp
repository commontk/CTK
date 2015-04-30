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


#include "ctkApplicationTestSuite_p.h"

#include <ctkPlugin.h>
#include <ctkPluginContext.h>

#include <ctkPluginFrameworkLauncher.h>

#include <ctkPluginFrameworkTestUtil.h>

#include <QTest>

//----------------------------------------------------------------------------
ctkApplicationTestSuite::ctkApplicationTestSuite(ctkPluginContext* pc)
  : pc(pc), p(pc->getPlugin())
{
}

//----------------------------------------------------------------------------
void ctkApplicationTestSuite::initTestCase()
{
  pS = ctkPluginFrameworkTestUtil::installPlugin(pc, "app_test");
  QVERIFY(pS);
}

//----------------------------------------------------------------------------
void ctkApplicationTestSuite::cleanupTestCase()
{
  pS->uninstall();
}

//----------------------------------------------------------------------------
void ctkApplicationTestSuite::runTest()
{
  try
  {
    pS->start();
    qDebug() << "started app plugin";
  }
  catch (const ctkPluginException& pexcS)
  {
    QString msg = QString("Test plugin: ") + pexcS.what();
    QFAIL(msg.toLatin1());
  }
}
