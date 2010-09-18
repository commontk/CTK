#ifndef CTKDICOMABSTRACTAPP_H
#define CTKDICOMABSTRACTAPP_H

#include <ctkDicomAppInterface.h>
#include <QScopedPointer>

class ctkDicomAbstractAppPrivate;

/**
  * Provide a basic implementation for an application app.
  *
  * It starts a http server and serves one hosted application. Multiple instances
  * can be used for hosting multiple applications.
  *
  * The methods of the ctkDicomAppInterface have to be implemented for the business logic,
  *
  */
class ctkDicomAbstractApp : public ctkDicomAppInterface
{
public:
    /**
      * Start the soap sever on the specified port or choose port automatically.
      */
    ctkDicomAbstractApp(int port = 0);
    int getPort() const;
    ~ctkDicomAbstractApp();

private:
    Q_DECLARE_PRIVATE(ctkDicomAbstractApp)
    const QScopedPointer<ctkDicomAbstractAppPrivate> d_ptr;

};

#endif // CTKDICOMABSTRACTAPP_H
