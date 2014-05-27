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

// Qt includes
#include <QtPlugin>
#include <QStringList>
#include <QString>

// CTK includes
#include "ctkExampleDicomAppPlugin_p.h"
#include "ctkExampleDicomAppLogic_p.h"

ctkPluginContext* ctkExampleDicomAppPlugin::Context = 0;

//----------------------------------------------------------------------------
ctkExampleDicomAppPlugin::ctkExampleDicomAppPlugin()
  : AppLogic(0)
{
}

//----------------------------------------------------------------------------
ctkExampleDicomAppPlugin::~ctkExampleDicomAppPlugin()
{
  qDebug()<< "delete applogic";
  delete this->AppLogic;
  this->AppLogic = 0;
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppPlugin::start(ctkPluginContext* context)
{
  ctkExampleDicomAppPlugin::Context = context;

  delete this->AppLogic;
  this->AppLogic = new ctkExampleDicomAppLogic();
  context->registerService<ctkDicomAppInterface>(this->AppLogic);
}

//----------------------------------------------------------------------------
void ctkExampleDicomAppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  ctkExampleDicomAppPlugin::Context = 0;
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkExampleDicomAppPlugin::getPluginContext()
{
  return ctkExampleDicomAppPlugin::Context;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_example_dicomapp, ctkExampleDicomAppPlugin)
#endif
