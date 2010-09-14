#include "ctkDicomAbstractApp.h"
#include "ctkDicomAppServer.h"

class ctkDicomAbstractAppPrivate
{
  public:
  ctkDicomAbstractAppPrivate(ctkDicomAbstractApp* appInterface, int port) : port(port)
  {
    // start server
    if (!port)
    {
      port = 8080;
    }
    server = new ctkDicomAppServer(appInterface,port);
  }
  ~ctkDicomAbstractAppPrivate()
  {
    delete server;
  }

  int port;
  ctkDicomAppServer* server;
};

ctkDicomAbstractApp::ctkDicomAbstractApp(int port) : d_ptr(new ctkDicomAbstractAppPrivate(this,port))
{
}

int ctkDicomAbstractApp::getPort() const
{
  Q_D(const ctkDicomAbstractApp);
  return d->port;
}
