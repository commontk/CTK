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

// CTK includes
#include "ctkDicomHostPlugin_p.h"

ctkDicomHostPlugin* ctkDicomHostPlugin::Instance = 0;

//----------------------------------------------------------------------------
ctkDicomHostPlugin::ctkDicomHostPlugin()
  : Context(0)
{
}

//----------------------------------------------------------------------------
ctkDicomHostPlugin::~ctkDicomHostPlugin()
{

}

//----------------------------------------------------------------------------
void ctkDicomHostPlugin::start(ctkPluginContext* context)
{
  ctkDicomHostPlugin::Instance = this;
  this->Context = context;
}

//----------------------------------------------------------------------------
void ctkDicomHostPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

//----------------------------------------------------------------------------
ctkDicomHostPlugin* ctkDicomHostPlugin::getInstance()
{
  return ctkDicomHostPlugin::Instance;
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkDicomHostPlugin::getPluginContext() const
{
  return this->Context;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_dah_host, ctkDicomHostPlugin)
#endif
