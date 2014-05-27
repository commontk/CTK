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

// CTK includes
#include "ctkDicomAppPlugin_p.h"
#include "ctkDicomAppServer_p.h"
#include "ctkDicomHostService_p.h"

// STD includes
#include <stdexcept>
#include <iostream>

ctkPluginContext* ctkDicomAppPlugin::Context = 0;

//----------------------------------------------------------------------------
ctkDicomAppPlugin::ctkDicomAppPlugin()
  : AppServer(0), HostInterface(0)
{

}

//----------------------------------------------------------------------------
ctkDicomAppPlugin::~ctkDicomAppPlugin()
{
  delete this->AppServer;
  delete this->HostInterface;
  ctkDicomAppPlugin::Context = 0;
}

//----------------------------------------------------------------------------
void ctkDicomAppPlugin::start(ctkPluginContext* context)
{
  bool canStart = true;

  ctkDicomAppPlugin::Context = context;


  QUrl appURL(context->getProperty("dah.appURL").toString());
  if (!appURL.isValid())
    {
    //throw ctkRuntimeException("The plugin framework does not contain a valid \"dah.appURL\" property");
    qDebug() << "ctkDicomAppPlugin: The plugin framework does not contain a valid \"dah.appURL\" property";
    canStart = false;
    }

  QUrl hostURL(context->getProperty("dah.hostURL").toString());
  if (!hostURL.isValid())
    {
    //throw ctkRuntimeException("The plugin framework does not contain a valid \"dah.hostURL\" property");
    qDebug() << "ctkDicomAppPlugin: The plugin framework does not contain a valid \"dah.hostURL\" property";
    canStart = false;
    }

  if(canStart)
    {
    // start the application server
    this->AppServer = new ctkDicomAppServer(appURL.port(), appURL.path());

    // register the host service, providing callbacks to the hosting application
    this->HostInterface = new ctkDicomHostService(QUrl(hostURL).port(), hostURL.path());
    context->registerService<ctkDicomHostInterface>(HostInterface);
    }
}

//----------------------------------------------------------------------------
void ctkDicomAppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)

  delete this->AppServer;
  delete this->HostInterface;
  this->AppServer = 0;
  this->HostInterface = 0;
  this->Context = 0;
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkDicomAppPlugin::getPluginContext()
{
  return ctkDicomAppPlugin::Context;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_dah_hostedapp, ctkDicomAppPlugin)
#endif
