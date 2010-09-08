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


#include "ctkDicomWG23HostPlugin_p.h"
#include <QtPlugin>

ctkDicomWG23HostPlugin* ctkDicomWG23HostPlugin::instance = 0;

ctkDicomWG23HostPlugin::ctkDicomWG23HostPlugin()
  : context(0)
{
}

ctkDicomWG23HostPlugin::~ctkDicomWG23HostPlugin()
{
  
}

void ctkDicomWG23HostPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;
}

void ctkDicomWG23HostPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

ctkDicomWG23HostPlugin* ctkDicomWG23HostPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkDicomWG23HostPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_dicom_wg23_host, ctkDicomWG23HostPlugin)


