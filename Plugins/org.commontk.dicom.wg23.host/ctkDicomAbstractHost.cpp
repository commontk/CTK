#include "ctkDicomAbstractHost.h"
#include "ctkDicomHostServer.h"

class ctkDicomAbstractHostPrivate
{
  public:
  ctkDicomAbstractHostPrivate(int port) : port(port)
  {
    // start server

  }

  int port;

};

ctkDicomAbstractHost::ctkDicomAbstractHost(int port) : d_ptr(new ctkDicomAbstractHostPrivate(port))
{
}

int ctkDicomAbstractHost::getPort() const
{
  Q_D(const ctkDicomAbstractHost);
  return d->port;
}
