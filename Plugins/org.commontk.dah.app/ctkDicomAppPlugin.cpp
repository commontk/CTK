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


#include "ctkDicomAppPlugin_p.h"
#include "ctkDicomAppServer_p.h"
#include "ctkDicomHostService_p.h"

#include <QtPlugin>
#include <QStringList>

#include <stdexcept>

ctkDicomAppPlugin* ctkDicomAppPlugin::instance = 0;

ctkDicomAppPlugin::ctkDicomAppPlugin()
  : context(0), appServer(0), hostInterface(0)
{

}

ctkDicomAppPlugin::~ctkDicomAppPlugin()
{
  
}

void ctkDicomAppPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;


  QUrl appURL(context->getProperty("dah.appURL").toString());
  if (!appURL.isValid())
  {
    throw std::runtime_error("The plugin framework does not contain a valid \"dah.appURL\" property");
  }

  QUrl hostURL(context->getProperty("dah.hostURL").toString());
  if (!hostURL.isValid())
  {
    throw std::runtime_error("The plugin framework does not contain a valid \"dah.hostURL\" property");
  }

  // start the application server
  appServer = new ctkDicomAppServer(appURL.port());

  // register the host service, providing callbacks to the hosting application
  hostInterface = new ctkDicomHostService(QUrl(hostURL).port(), "/HostInterface");
  context->registerService(QStringList("ctkDicomHostInterface"), hostInterface);

}

void ctkDicomAppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  delete appServer;
  delete hostInterface;
}

ctkDicomAppPlugin* ctkDicomAppPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkDicomAppPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_dah_app, ctkDicomAppPlugin)


