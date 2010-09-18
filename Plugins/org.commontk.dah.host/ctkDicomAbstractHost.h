#ifndef CTKDICOMABSTRACTHOST_H
#define CTKDICOMABSTRACTHOST_H

#include <ctkDicomHostInterface.h>
#include <ctkDicomAppInterface.h>
#include <QScopedPointer>

#include <org_commontk_dah_host_Export.h>

class ctkDicomAbstractHostPrivate;

/**
  * Provide a basic implementation for an application host.
  *
  * It starts a http server and serves one hosted application. Multiple instances
  * can be used for hosting multiple applications.
  *
  * The methods of the ctkDicomHostInterface have to be implemented for the business logic,
  *
  */
class org_commontk_dah_host_EXPORT ctkDicomAbstractHost : public ctkDicomHostInterface
{
public:
    /**
      * Start the soap sever on the specified port or choose port automatically.
      */
    ctkDicomAbstractHost(int hostPort = 0, int appPort = 0);
    int getHostPort() const;
    int getAppPort() const;
    ~ctkDicomAbstractHost();

    ctkDicomAppInterface* getDicomAppService() const;
private:
    Q_DECLARE_PRIVATE(ctkDicomAbstractHost)
    const QScopedPointer<ctkDicomAbstractHostPrivate> d_ptr;

};

#endif // CTKDICOMABSTRACTHOST_H
