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


#include "ctkTestAppActivator_p.h"
#include "ctkTestApp_p.h"

#include "ctkTestApp_p.h"

#include <ctkPluginContext.h>

#include <QtGlobal>
#include <QDebug>

//----------------------------------------------------------------------------
ctkTestAppActivator::ctkTestAppActivator()
{
}

//----------------------------------------------------------------------------
ctkTestAppActivator::~ctkTestAppActivator()
{
}

//----------------------------------------------------------------------------
void ctkTestAppActivator::start(ctkPluginContext* context)
{
  qDebug() << "[app_test] ctkTestAppActivator::start()";

  appContainer.reset(new MyAppContainer(context));
  appContainer->start();
}

//----------------------------------------------------------------------------
void ctkTestAppActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  appContainer->stop();
}
