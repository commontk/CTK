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

#include "ctkDicomAbstractHost.h"
#include "ctkDicomHostServer.h"
#include "ctkDicomAppService.h"

class ctkDicomAbstractHostPrivate
{
public:

  ctkDicomAbstractHostPrivate(ctkDicomAbstractHost* hostInterface, int hostPort, int appPort) : hostPort(hostPort), appPort(appPort)
  {
    // start server
    if (hostPort==0)
    {
      hostPort = 8080;
    }
    if (appPort==0)
    {
      appPort = 8081;
    }

    server = new ctkDicomHostServer(hostInterface,hostPort);
    appService = new ctkDicomAppService(appPort, "/ApplicationInterface");
  }

  ~ctkDicomAbstractHostPrivate()
  {
    delete server;
    delete appService;
  }

  int hostPort;
  int appPort;
  ctkDicomHostServer* server;
  ctkDicomAppInterface* appService;
  // ctkDicomAppHosting::Status
};

ctkDicomAbstractHost::ctkDicomAbstractHost(int hostPort, int appPort) : d_ptr(new ctkDicomAbstractHostPrivate(this,hostPort,appPort))
{
}

int ctkDicomAbstractHost::getHostPort() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->hostPort;
}

int ctkDicomAbstractHost::getAppPort() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->appPort;
}

ctkDicomAbstractHost::~ctkDicomAbstractHost()
{
}

ctkDicomAppInterface* ctkDicomAbstractHost::getDicomAppService() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->appService;
}
