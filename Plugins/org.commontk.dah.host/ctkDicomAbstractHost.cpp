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

  ctkDicomAbstractHostPrivate(ctkDicomAbstractHost* hostInterface, int HostPort, int AppPort);
  ~ctkDicomAbstractHostPrivate();

  int HostPort;
  int AppPort;
  ctkDicomHostServer* Server;
  ctkDicomAppInterface* AppService;
  // ctkDicomAppHosting::Status
};

//----------------------------------------------------------------------------
// ctkDicomAbstractHostPrivate methods

//----------------------------------------------------------------------------
ctkDicomAbstractHostPrivate::ctkDicomAbstractHostPrivate(
  ctkDicomAbstractHost* hostInterface, int hostPort, int appPort) : HostPort(hostPort), AppPort(appPort)
{
  // start server
  if (this->HostPort==0)
    {
    this->HostPort = 8080;
    }
  if (this->AppPort==0)
    {
    this->AppPort = 8081;
    }

  this->Server = new ctkDicomHostServer(hostInterface,hostPort);
  this->AppService = new ctkDicomAppService(appPort, "/ApplicationInterface");
}

//----------------------------------------------------------------------------
ctkDicomAbstractHostPrivate::~ctkDicomAbstractHostPrivate()
{
  delete this->Server;
  this->Server = 0;
  //do not delete AppService, deleted somewhere else before?
  //delete  this->AppService;
  //this->AppService = 0;
}

//----------------------------------------------------------------------------
// ctkDicomAbstractHost methods

//----------------------------------------------------------------------------
ctkDicomAbstractHost::ctkDicomAbstractHost(int hostPort, int appPort) :
  d_ptr(new ctkDicomAbstractHostPrivate(this, hostPort, appPort))
{
}

//----------------------------------------------------------------------------
int ctkDicomAbstractHost::getHostPort() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->HostPort;
}

//----------------------------------------------------------------------------
int ctkDicomAbstractHost::getAppPort() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->AppPort;
}

//----------------------------------------------------------------------------
ctkDicomAbstractHost::~ctkDicomAbstractHost()
{
}

//----------------------------------------------------------------------------
ctkDicomAppInterface* ctkDicomAbstractHost::getDicomAppService() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->AppService;
}
