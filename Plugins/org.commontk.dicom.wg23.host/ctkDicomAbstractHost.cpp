#include "ctkDicomAbstractHost.h"
#include "ctkDicomHostServer.h"

class ctkDicomAbstractHostPrivate
{
  public:
  ctkDicomAbstractHostPrivate(ctkDicomAbstractHost* hostInterface, int port) : port(port)
  {
    // start server
    if (!port)
    {
      port = 8080;
    }
    server = new ctkDicomHostServer(hostInterface,port);
  }
  ~ctkDicomAbstractHostPrivate()
  {
    delete server;
  }

  int port;
  ctkDicomHostServer* server;
  // ctkDicomWG23::Status
};

ctkDicomAbstractHost::ctkDicomAbstractHost(int port) : d_ptr(new ctkDicomAbstractHostPrivate(this,port))
{
}

int ctkDicomAbstractHost::getPort() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->port;
}

ctkDicomAbstractHost::~ctkDicomAbstractHost()
{
}
