/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#include "ctkDicomExampleHostPlugin_p.h"
#include <QtPlugin>

ctkDicomExampleHostPlugin* ctkDicomExampleHostPlugin::instance = 0;

ctkDicomExampleHostPlugin::ctkDicomExampleHostPlugin()
  : context(0)
{
}

ctkDicomExampleHostPlugin::~ctkDicomExampleHostPlugin()
{
  
}

void ctkDicomExampleHostPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;
}

void ctkDicomExampleHostPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

ctkDicomExampleHostPlugin* ctkDicomExampleHostPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkDicomExampleHostPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_dah_examplehost_Export_Export, ctkDicomExampleHostPlugin)


