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
  // ctkDicomWG23::Status
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
