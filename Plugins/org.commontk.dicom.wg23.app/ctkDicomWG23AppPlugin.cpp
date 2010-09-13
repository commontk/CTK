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


#include "ctkDicomWG23AppPlugin_p.h"

#include <ctkDicomHostServicePrivate.h>

#include <QtPlugin>

ctkDicomWG23AppPlugin* ctkDicomWG23AppPlugin::instance = 0;

ctkDicomWG23AppPlugin::ctkDicomWG23AppPlugin()
  : context(0)
{

}

ctkDicomWG23AppPlugin::~ctkDicomWG23AppPlugin()
{
  
}

void ctkDicomWG23AppPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;

  hostInterface = new ctkDicomHostService();
  context->registerService(QStringList("ctkDicomHostInterface"), hostInterface);
}

void ctkDicomWG23AppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  delete hostInterface;
}

ctkDicomWG23AppPlugin* ctkDicomWG23AppPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkDicomWG23AppPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_dicom_wg23_app, ctkDicomWG23AppPlugin)


