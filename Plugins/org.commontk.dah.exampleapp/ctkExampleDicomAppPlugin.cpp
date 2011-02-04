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


#include "ctkExampleDicomAppPlugin_p.h"
#include "ctkExampleDicomAppLogic_p.h"
#include <QtPlugin>
#include <QStringList>
#include <QString>

ctkPluginContext* ctkExampleDicomAppPlugin::context = 0;

ctkExampleDicomAppPlugin::ctkExampleDicomAppPlugin()
  : appLogic(0)
{
}

ctkExampleDicomAppPlugin::~ctkExampleDicomAppPlugin()
{
  delete appLogic;
}

void ctkExampleDicomAppPlugin::start(ctkPluginContext* context)
{
  this->context = context;

  delete appLogic;
  appLogic = new ctkExampleDicomAppLogic();
  context->registerService<ctkDicomAppInterface>(appLogic);
}

void ctkExampleDicomAppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  this->context = 0;
}

ctkPluginContext* ctkExampleDicomAppPlugin::getPluginContext()
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_example_dicomapp, ctkExampleDicomAppPlugin)


